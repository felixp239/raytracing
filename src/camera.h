#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>

#include "hittable.h"
#include "material.h"

void write_progress_bar(int current_percentage) {
    int barWidth = 50;

    if (current_percentage >= 100) {
        std::clog << "[";
        int pos = barWidth * current_percentage / 100;
        for (int i = 0; i < barWidth; ++i) {
            std::clog << "=";
        }
        std::clog << "] ";
        std::clog << 100 << " %\n" << std::flush;
        return;
    }

    std::clog << "[";
    int pos = barWidth * current_percentage / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::clog << "=";
        else if (i == pos) std::clog << ">";
        else std::clog << " ";
    }
    std::clog << "] ";
    if (current_percentage < 10) {
        std::clog << ' ';
    }
    std::clog << ' ' << current_percentage << " %\r" << std::flush;
}

class camera {
    public:
        double aspect_ratio      = 1.0;
        int    image_width       = 100;
        int    samples_per_pixel = 10;
        int    max_depth         = 10;
        
        double vfov              = 90; 
        point3 lookfrom = point3(0, 0, 0);   // Point camera is looking from
        point3 lookat = point3(0, 0, -1);  // Point camera is looking at
        vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction

        void render(const hittable& world) {
            std::ofstream   output_file;
            std::string     output_filename;

            initialize();

            sf::RenderWindow window(sf::VideoMode(1440, 810), "Raytracer");

            sf::Uint32* sample_buffer = new sf::Uint32[image_width * image_height * 4];

            clearSampleBuffer(sample_buffer);

            int current_samples = 0;

            sf::Uint8* pixels = new sf::Uint8[image_width * image_height * 4];

            sf::Texture texture;
            texture.create(image_width, image_height);
            sf::Sprite sprite(texture);
            
            sprite.setScale(1440.0 / image_width, 1440.0 / image_width);
          
            texture.update(pixels);

            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                }

                updateSampleBuffer(world, sample_buffer);

                current_samples += 1;

                updatePixels(pixels, sample_buffer, current_samples);

                texture.update(pixels);

                window.clear();
                window.draw(sprite);
                window.display();
            }
        }

    private:
        int    image_height;
        double pixels_samples_scale;
        point3 camera_center;
        point3 start_pixel;
        vec3   pixel_delta_u;
        vec3   pixel_delta_v;
        vec3   u, v, w;              // Camera frame basis vectors

        void updateSampleBuffer(const hittable& world, sf::Uint32* sample_buffer)
        {
            for (int j = 0; j < image_height; j++) {
                for (int i = 0; i < image_width; i++) {
                    ray r = get_ray(i, j);
                    color pixel_color = ray_color(r, max_depth, world);

                    static const interval intensity{ 0, 0.999 };

                    sample_buffer[4 * (j * image_width + i) + 0] += int(256 * intensity.clamp(linear_to_gamma(pixel_color.x())));
                    sample_buffer[4 * (j * image_width + i) + 1] += int(256 * intensity.clamp(linear_to_gamma(pixel_color.y())));
                    sample_buffer[4 * (j * image_width + i) + 2] += int(256 * intensity.clamp(linear_to_gamma(pixel_color.z())));
                    sample_buffer[4 * (j * image_width + i) + 3] += 255;
                }
            }
        }

        // Gamma correction from linear space to gamma 2 (whatever the hell that means)
        inline double linear_to_gamma(double linear_component)
        {
            if (linear_component > 0)
                return sqrt(linear_component);
            return 0;
        }

        void clearSampleBuffer(sf::Uint32* sample_buffer) {
            for (int j = 0; j < image_height; j++) {
                for (int i = 0; i < image_width; i++) {
                    sample_buffer[4 * (j * image_width + i) + 0] = 0;
                    sample_buffer[4 * (j * image_width + i) + 1] = 0;
                    sample_buffer[4 * (j * image_width + i) + 2] = 0;
                    sample_buffer[4 * (j * image_width + i) + 3] = 0;
                }
            }
        }

        void updatePixels(sf::Uint8* pixels, const sf::Uint32* sample_buffer, int current_samples) {
            for (int i = 0; i < 4 * image_width * image_height; i++) {
                pixels[i] = sample_buffer[i] / current_samples;
            }
        }

        void initialize() {
            // Calculate the image height, and ensure that it's at least 1.
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixels_samples_scale = 1.0 / samples_per_pixel;

            // Camera 
            double focal_length     = (lookat - lookfrom).length();
            double theta            = degrees_to_radians(vfov);
            double h                = tan(theta / 2);
            double viewport_height  = 2 * h * focal_length;
            double viewport_width   = viewport_height * image_width / image_height;
            camera_center           = lookfrom;

            w = -(lookat - lookfrom) / focal_length;
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            vec3 viewport_u = viewport_width * u;
            vec3 viewport_v = viewport_height * -v;

            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            pixel_delta_u   = viewport_u / image_width;
            pixel_delta_v   = viewport_v / image_height;

            // Calculate the location of the upper left pixel.
            point3 viewport_upper_left = camera_center
                                    - focal_length * w
                                    - 0.5 * (viewport_u + viewport_v);
            start_pixel                = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        }

        ray get_ray(int i, int j) const {
            // Construct a camera ray originating from the origin and directed at randomly sampled
            // point around the pixel location i, j.

            vec3   offset       = sample_square();
            point3 pixel_sample = start_pixel
                                + ((i + offset.x()) * pixel_delta_u)
                                + ((j + offset.y()) * pixel_delta_v);
            
            point3 ray_origin    = camera_center;
            vec3   ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction); // !!!ray_direction is NOT normalized!!!
        }

        vec3 sample_square() const {
            // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
            return vec3(random_double(-0.5, 0.5), random_double(-0.5, 0.5), 0);
        }

        color ray_color(const ray& r, int depth, const hittable& world) const {
            if (depth <= 0) {
                return color(0, 0, 0);
            }

            hit_record rec;
            if (world.hit(r, interval(0.001, infinity), rec)) {
                ray scattered;
                color attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                    return attenuation * ray_color(scattered, depth - 1, world);
                }
                return color(0, 0, 0);
            }

            vec3 unit_direction = unit_vector(r.direction());
            static vec3     gradient_direction  = unit_vector(vec3(1, 3, 0));
            static color    color_start         = color{ 1.0, 1.0, 1.0 };
            static color    color_finish        = color{ 0.5, 1.0, 0.7 };
            return skybox_gradient(unit_direction, gradient_direction, color_start, color_finish);
        }

        color skybox_gradient(const vec3& unit_direction, const vec3& gradient_direction,
                              const color& color_start, const color& color_finish) const {
            double a = 0.5 * (dot(unit_direction, gradient_direction) + 1.0);
            return (1 - a) * color_start + a * color_finish;
        }
};

#endif