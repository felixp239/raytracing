#include "common.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"


int main() {
    // World 

    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera 

    camera cam;

    double aspect_ratio = 16.0 / 9.0;
    cam.aspect_ratio    = aspect_ratio;

    int image_width;

    std::cout << "Give an image width: ";
    std::cin >> image_width;

    cam.image_width = image_width;

    int samples_per_pixel;

    std::cout << "Give an amount of samples per pixel: ";
    std::cin >> samples_per_pixel;

    cam.samples_per_pixel = samples_per_pixel;
    
    cam.render(world);
    
    return 0;
}