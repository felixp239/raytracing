#include "vec3.h"
#include "ray.h"
#include "color.h"

#include <iostream>
#include <fstream>

double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = center - r.origin();
    double a = r.direction().length_squared();
    double h = dot(r.direction(), oc);
    double c = oc.length_squared() - radius * radius;
    double discriminant = h * h - a * c;
    
    if (discriminant < 0) {
        return -1.0;
    }
    else {
        return (h - sqrt(discriminant)) / a;
    }
}

color ray_color(const ray& r) {
    vec3 sphere_center = vec3(0, 0, -2);
    double radius = 0.75;
    double t = hit_sphere(sphere_center, radius, r);
    if (t > 0.0) {
        vec3 N = (r.at(t) - sphere_center) / radius;
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    vec3 gradient_direction = unit_vector(vec3(0, 1, 0));
    double a = 0.5 * (dot(unit_direction, gradient_direction) + 1.0);
    return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {
    double aspect_ratio = 16.0 / 9.0;

    int image_width;

    std::cout << "Give an image width: ";
    std::cin >> image_width;

    // Calculate the image height, and ensure that it's at least 1.
    int image_height    = int(image_width / aspect_ratio);
    image_height        = (image_height < 1) ? 1 : image_height;

    // Camera 

    double focal_length     = 1.0; 
    double viewport_height  = 2.0;
    double viewport_width   = viewport_height * image_width / image_height;
    point3 camera_center    = point3{ 0, 0, 0 };

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    point3 viewport_upper_left = camera_center
                             - vec3(0, 0, focal_length)
                             - 0.5 * (viewport_u + viewport_v);
    point3 start_pixel         = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    std::ofstream   output_file;
    std::string     output_filename;

    while (!output_file.is_open()) {
        std::cout << "Give an output filename: ";
        std::cin >> output_filename;
        output_filename = "../results/" + output_filename + ".ppm";
        output_file.open(output_filename);
    }
    
    std::clog << "Process start\n";

    output_file << "P3" << '\n';
    output_file << image_width << ' ' << image_height << '\n';
    output_file << 255 << '\n';

    if ((image_height <= 0) || (image_width <= 0)) {
        return 1;
    }
    
    int p = 1;
    int image_size = image_height * image_width;

    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            point3 current_pixel = start_pixel + x * pixel_delta_u + y * pixel_delta_v;
            vec3   ray_direction = current_pixel - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(output_file, pixel_color);

            if (p * image_size < 100 * (y * image_width + x + 1)) {
                if (p < 10) {
                    std::clog << " ";
                }
                std::clog << " " << p << "% done\n" << std::flush;
                p += 1;
            }
        }
    }

    std::clog << p << "% done\n";

    output_file.close();

    std::clog << "Your file is done!\n" << std::flush;

    return 0;
}