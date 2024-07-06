#ifndef VEC3_H  
#define VEC3_H

class vec3 {
    public:
        double e[3];
        
        vec3() : e{0, 0, 0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }
        
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3& v) {
            e[0] += v[0];
            e[1] += v[1];
            e[2] += v[2];
            return *this;
        }

        vec3& operator*=(double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(double t) {
            return *this *= 1 / t;
        }

        double length() const {
            return sqrt(length_squared());
        }

        double length_squared() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        static vec3 random() {
            return vec3{ random_double(), random_double(), random_double() };
        }

        static vec3 random(double min, double max) {
            return vec3{ random_double(min, max), random_double(min, max), random_double(min, max) };
        }

        bool near_zero() const {
            // Return true if the vector is close to zero in all dimensions.
            double s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

inline vec3 operator+(const vec3& u, const vec3 v) {
    return vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

inline vec3 operator-(const vec3& u, const vec3 v) {
    return vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

inline vec3 operator*(const vec3& u, const vec3 v) {
    return vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v[0], t * v[1], t * v[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u[0] * v[0] 
         + u[1] * v[1] 
         + u[2] * v[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u[1] * v[2] - u[2] * v[1],
                u[2] * v[0] - u[0] * v[2],
                u[0] * v[1] - u[1] * v[0]);
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while (true) {
        vec3 p = vec3(random_double(-1.0, 1.0), random_double(-1.0, 1.0), 0);
        if (p.length_squared() <= 1) {
            return unit_vector(p);
        }
    }
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        vec3 p = vec3::random(-1, 1);
        if (p.length_squared() <= 1) {
            return p;
        }
    }
}

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) { // In same hemisphere as the normal
        return on_unit_sphere;
    }
    return -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& normal) {
    return v - 2 * dot(v, normal) * normal;
}

inline vec3 refract(const vec3& r_in, const vec3& normal, double refractive_from_over_refractive_in) {
    double cos_theta = fmin(dot(-r_in, normal), 1);
    vec3 r_out_perpendicular = refractive_from_over_refractive_in * (r_in + cos_theta * normal);
    vec3 r_out_parallel = -sqrt(fabs(1 - r_out_perpendicular.length_squared())) * normal;
    return r_out_perpendicular + r_out_parallel;
}

#endif