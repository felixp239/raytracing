#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>

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

        void updateSampleBuffer(const hittable& world, sf::Uint32* sample_buffer)
        {
            for (int j = 0; j < image_height; j++) {
                for (int i = 0; i < image_width; i++) {
                    ray r = get_ray(i, j);
                    color pixel_color = ray_color(r, max_depth, world);

                    static const interval intensity{ 0, 0.999 };

                    // We apply sqrt to pixels_color because that way we trans
                    sample_buffer[4 * (j * image_width + i) + 0] += int(256 * intensity.clamp(linear_to_gamma(pixel_color.x())));
                    sample_buffer[4 * (j * image_width + i) + 1] += int(256 * intensity.clamp(linear_to_gamma(pixel_color.y())));
                    sample_buffer[4 * (j * image_width + i) + 2] += int(256 * intensity.clamp(linear_to_gamma(pixel_color.z())));
                    sample_buffer[4 * (j * image_width + i) + 3] += 255;
                }
            }
        }

        // Gamma correction (whatever the hell that means)
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
            double focal_length     = 1.0; 
            double viewport_height  = 2.0;
            double viewport_width   = viewport_height * image_width / image_height;
            camera_center           = point3{ 0, 0, 0 };

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            vec3 viewport_u = vec3(viewport_width, 0, 0);
            vec3 viewport_v = vec3(0, -viewport_height, 0);

            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            pixel_delta_u   = viewport_u / image_width;
            pixel_delta_v   = viewport_v / image_height;

            // Calculate the location of the upper left pixel.
            point3 viewport_upper_left = camera_center
                                    - vec3(0, 0, focal_length)
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
                vec3 direction = rec.normal + random_unit_vector(); // !!!This is NOT normalized!!!
                return 0.5 * ray_color(ray(rec.p, direction), depth - 1, world);
            }

            vec3 unit_direction = unit_vector(r.direction());
            /*if (unit_direction[0] > 0 && unit_direction[1] > 0.5 && unit_direction[2] < 0 && unit_direction[0] < 0.5 && unit_direction[2] > -0.7) {
                return color(1.0, 1.0, 1.0);
            }*/
            vec3 gradient_direction = unit_vector(vec3(0, 1, 0));
            double a = 0.5 * (dot(unit_direction, gradient_direction) + 1.0);
            return (1 - a) * color(1.0, 1.0, 1.0) + a * color(1.0, 0.07, 0.57);
        }

};

#endif