#include "common.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "triangle.h"


void create_world_1(hittable_list& world) {
    shared_ptr<material> material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    shared_ptr<material> material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    shared_ptr<material> material_left = make_shared<dielectric>(1.50);
    shared_ptr<material> material_bubble = make_shared<dielectric>(1.00 / 1.50);
    shared_ptr<material> material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.9);
    shared_ptr<material> material_back = make_shared<metal>(color(0.8, 0.6, 0.2), 0.01);

    world.add(make_shared<sphere>(point3(0.0, -10000.55, -1.0), 10000.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.48, material_bubble));
    world.add(make_shared<sphere>(point3(-3, 0.0, -4.0), 0.5, material_back));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
}

void create_world_2(hittable_list& world) {
    auto R = cos(pi / 4);

    auto material_left = make_shared<lambertian>(color(0, 0, 1));
    auto material_right = make_shared<lambertian>(color(1, 0, 0));

    world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_left));
    world.add(make_shared<sphere>(point3(R, 0, -1), R, material_right));
}

void create_world_3(hittable_list& world) {
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
}

void create_world_4(hittable_list& world) {
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_right = make_shared<metal>(color(1, 0.5, 0.5), 0.00);

    world.add(make_shared<sphere>(point3(-1.0, 0.0, -0.7), 0.5, material_ground));

    world.add(make_shared<triangle>(point3(0.5, -0.5, -1.5), point3(-4, -0.5, -1.0), point3(-4, 10, -1.0), material_right));
    world.add(make_shared<triangle>(point3(0.5, -0.5, -1.5), point3(-4, 10, -1.0), point3(-4, 10, -1.0), material_right));
}

int main() {
    hittable_list world;

    create_world_4(world);

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
    
    cam.vfov = 60;

    cam.lookfrom = point3(0, 0, 0);
    cam.lookat = point3(-0.5, 0, -1);
    cam.vup = vec3(0, 1, 0);

    cam.render(world);
    
    return 0;
}