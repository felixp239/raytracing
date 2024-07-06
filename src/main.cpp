#include "common.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"


int main() {
    hittable_list world;

    shared_ptr<material> material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    shared_ptr<material> material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    shared_ptr<material> material_left   = make_shared<dielectric>(1.50);
    shared_ptr<material> material_bubble = make_shared<dielectric>(1.00 / 1.50);
    shared_ptr<material> material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.9);
    shared_ptr<material> material_back   = make_shared<metal>(color(0.8, 0.6, 0.2), 0.01);

    world.add(make_shared<sphere>(point3(0.0, -10000.55, -1.0), 10000.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.48, material_bubble));
    world.add(make_shared<sphere>(point3(-3, 0.0, -4.0), 0.5, material_back));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    camera cam;

    double aspect_ratio = 16.0 / 9.0;
    cam.aspect_ratio    = aspect_ratio;

    int image_width;

    std::cout << "Give an image width: ";
    std::cin >> image_width;

    cam.image_width = image_width;

    int samples_per_pixel;

    //std::cout << "Give an amount of samples per pixel: ";
    //std::cin >> samples_per_pixel;

    cam.max_depth = 100;

    cam.samples_per_pixel = 1;
    
    cam.render(world);
    
    return 0;
}