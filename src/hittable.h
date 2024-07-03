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
        double t;
        const hittable& object;

        vec3 normal() const {
            return object.normal(p);
        }
};

#endif