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
    
    std::cout << "Process start\n";

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
            auto r = x * 255.0 / (image_width - 1);
            auto g = y * 255.0 /  (image_height - 1);
            auto b = 0.0;

            int ir = int(r);
            int ig = int(g);
            int ib = int(b);

            output_file << ir << ' ' << ig << ' ' << ib << '\n';
            
            if (p * image_size < 100 * (y * image_width + x + 1)) {
                if (p < 10) {
                    std::cout << " ";
                }
                std::cout << " " << p << "% done\n";
                p += 1;
            }
        }
    }

    std::cout << p << "% done\n";

    output_file.close();

    std::cout << "Your file is done!\n";

    return 0;
}