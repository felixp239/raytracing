#include "vec3.h"
#include "color.h"

#include <iostream>
#include <fstream>

int main() {
    int image_width     = 256;
    int image_height    = 256;

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
            color pixel_color = color(double(x) / (image_width - 1), double(y) / (image_height - 1), 0);
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