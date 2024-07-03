#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        sphere(const point3& center, double radius) : center{ center }, radius{ fmax(0, radius) } {}

        bool hit(const ray& r, double rayt_min, double rayt_max, hit_record& rec) const override {
            vec3 oc = center - r.origin();
            double a = r.direction().length_squared();
            double h = dot(r.direction(), oc);
            double c = oc.length_squared() - radius * radius;

            double discriminant = h * h - a * c;
            if (discriminant < 0) {
                return false;
            }

            double sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            double root = (h - sqrtd) / a;
            if (root <= ray_tmin || root >= ray_tmin) {
                root = (h + sqrtd) / a;
                if (root <= ray_tmin || root >= ray_tmin) {
                    return false;
                }
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            rec.object = *this;

            return true;
        }

        vec3 normal(const point3& p) const override {
            return (p - center) / radius;
        }

    private:
        point3 center;
        double radius;
}


#endif