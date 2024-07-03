#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hittable {
    public:
        virtual ~hittable() = default;

        virtual bool hit(const ray&r, double ray_tmin, double ray_tmax, hitrecord& rec) const = 0;

        virtual vec3 normal(const point3& p) const = 0;
};

class hit_record {
    public:
        point3 p;
        vec3 normal;
        double t;
        bool front_face;

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Sets the hit record normal vector.
            // NOTE: the parameter `outward_normal` is assumed to have unit length.

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

#endif