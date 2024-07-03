#ifndef CAMERA_H
#define CAMERA_H

class camera {
    public:
        double aspect_ratio = 1.0;
        int    image_width  = 100;
        
        void render(const hittable& world) {
            std::ofstream   output_file;
            std::string     output_filename;

            while (!output_file.is_open()) {
                std::cout << "Give an output filename: ";
                std::cin >> output_filename;
                output_filename = "../results/" + output_filename + ".ppm";
                output_file.open(output_filename);
            }

            initialize();

            std::clog << "Process start\n";

            output_file << "P3" << '\n';
            output_file << image_width << ' ' << image_height << '\n';
            output_file << 255 << '\n';

            if ((image_height <= 0) || (image_width <= 0)) {
                return;
            }
            
            int next_percentage = 1;
            int image_size = image_height * image_width;

            for (int y = 0; y < image_height; y++) {
                for (int x = 0; x < image_width; x++) {
                    point3 current_pixel = start_pixel + x * pixel_delta_u + y * pixel_delta_v;
                    vec3   ray_direction = current_pixel - camera_center;
                    ray r(camera_center, ray_direction);

                    color pixel_color = ray_color(r, world);
                    write_color(output_file, pixel_color);

                    int current_percentage = (y * image_width + x + 1) * 100 / image_size;
                    if (current_percentage >= next_percentage) {
                        if (current_percentage < 10) {
                            std::clog << ' ';
                        }
                        if (current_percentage < 100) {
                            std::clog << ' ';
                        }
                        std::clog << current_percentage << "% done\n" << std::flush;
                        next_percentage = current_percentage + 1;
                    }
                }
            }

            if (next_percentage <= 100) {
                std::clog << "100% done\n";
            }

            output_file.close();

            std::clog << "Your file is done!\n" << std::flush;
        }

    private:
        int    image_height;
        point3 camera_center;
        point3 start_pixel;
        vec3   pixel_delta_u;
        vec3   pixel_delta_v;

        void initialize() {
            // Calculate the image height, and ensure that it's at least 1.
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

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

        color ray_color(const ray& r, const hittable& world) const {
            hit_record rec;
            if (world.hit(r, interval(0, infinity), rec)) {
                return 0.5 * (rec.normal + color(1.0, 1.0, 1.0));
            }

            vec3 unit_direction = unit_vector(r.direction());
            vec3 gradient_direction = unit_vector(vec3(0, 1, 0));
            double a = 0.5 * (dot(unit_direction, gradient_direction) + 1.0);
            return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        }
};

#endif