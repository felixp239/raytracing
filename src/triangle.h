#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

class triangle : public hittable {
public:
    triangle(const point3& a, const point3& b, const point3& c, shared_ptr<material> mat) : points{ a, b, c }, mat{ mat } {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 ab = points[1] - points[0];
        vec3 ac = points[2] - points[0];

        vec3 normal = unit_vector(cross(ab, ac));

        double dot_ray_normal = dot(normal, r.direction());

        if (fabs(dot_ray_normal) < 1e-8) {
            return false;
        }

        /*if (dot_ray_normal > 0) {
            normal = -normal;
        }*/
        

        double C = dot(normal, points[0]);

        double root = (C - dot(normal, r.origin())) / dot(normal, r.direction());

        if (!ray_t.surrounds(root)) {
            return false;
        }

        point3 p = r.at(root);

        vec3 C0 = p - points[0];
        vec3 C1 = p - points[1];
        vec3 C2 = p - points[2];

        if (dot(normal, cross(points[1] - points[0], p - points[0])) <= 0 ||
            dot(normal, cross(points[2] - points[1], p - points[1])) <= 0 ||
            dot(normal, cross(points[0] - points[2], p - points[2])) <= 0) {
            return false;
        }

        rec.front_face = true;
        rec.mat = mat;
        rec.normal = (dot_ray_normal > 0) ? -normal : normal;
        rec.p = p;
        rec.t = root;

        return true;
    }

private:
    point3               points[3];
    shared_ptr<material> mat;
};



#endif // ! TRIANGLE_H