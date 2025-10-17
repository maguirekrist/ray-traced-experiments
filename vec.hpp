#pragma once

#include "constants.hpp"
#include <cmath>
#include <iostream>
#include <format>

class Vec3 {
public:
	double e[3];
	
	Vec3() {}

	Vec3(double x, double y, double z) : e{x, y, z} {} 

        double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
	
	Vec3& operator+=(const Vec3& v) {
		e[0] += v.x();
		e[1] += v.y();
		e[2] += v.z();
		return *this;
	}

	Vec3& operator*=(double t) {
		e[0] *= t; 
		e[1] *= t; 
		e[2] *= t;
		return *this;
	}

	Vec3& operator/=(double t) {
		return *this *= 1/t;
	}

	double length() const {
		return std::sqrt(length_squared());
	}

	double length_squared() const {
		return x()*x() + y()*y() + z()*z();
	}

	bool near_zero() const {
		auto s = 1e-8;
		return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
	}

	static Vec3 random() {
		return Vec3(random_double(), random_double(), random_double());
	}

	static Vec3 random(double min, double max) {
		return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}
};

using Point3D = Vec3;

inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
	return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline Vec3 operator+(const Vec3& u, const Vec3& v) {
	return Vec3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

inline Vec3 operator-(const Vec3& u, const Vec3& v) {
	return Vec3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}

inline Vec3 operator*(const Vec3& u, const Vec3& v) {
	return Vec3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}

inline Vec3 operator*(double t, const Vec3& v) {
	return Vec3(t*v.x(), t*v.y(), t*v.z());
}

inline Vec3 operator*(const Vec3& v, double t) {
	return t * v;
}

inline Vec3 operator/(const Vec3& v, double t) {
	return (1/t) * v;
}

inline double dot(const Vec3& u, const Vec3& v) {
	return u.x() * v.x()
		+ u.y() * v.y()
		+ u.z() * v.z();
}

inline Vec3 cross(const Vec3& u, const Vec3& v) {
	return Vec3(
		u.y() * v.z() - u.z() * v.y(),
		u.z() * v.x() - u.x() * v.z(),
		u.x() * v.y() - u.y() * v.x()
	     );
}

inline Vec3 unit_vector(const Vec3& v) {
	return v / v.length();
}

inline Vec3 random_unit_vector() {
	while (true)
	{
		auto p = Vec3::random(-1, 1);
		auto lensq = p.length_squared();
		if (1e-160 < lensq && lensq <= 1)
			return p / std::sqrt(lensq);
	}
}

inline Vec3 random_in_unit_disk() {
	while(true) {
		auto p = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() < 1)
			return p;
	}
}

inline Vec3 random_on_hemisphere(const Vec3& normal) {
	Vec3 on_unit_sphere = random_unit_vector();
	if (dot(on_unit_sphere, normal) > 0.0)
		return on_unit_sphere;
	else
		return -on_unit_sphere;
}

inline Vec3 random_sphereical_vector(double theta_low, double theta_high) {
	double random_phi = random_double(0.0, 2.0 * pi);
	double random_theta = random_double(theta_low, theta_high);

	double s = std::sin(random_theta);
	double x = std::cos(random_phi) * s;
	double y = std::sin(random_phi) * s; 
	double z = std::cos(random_theta);

	return Vec3(x, y, z);
}

inline Vec3 random_on_hemisphere2(const Vec3& normal) {
	//Create local coordinate system at the normal
	Vec3 n = unit_vector(normal);
	Vec3 up = std::fabs(n.y()) < 0.999 ? Vec3{0, 1, 0} : Vec3{0, 0, 1};
	Vec3 tangent = unit_vector(cross(up, n));
	Vec3 bitangent = cross(n, tangent);

	//Sample a valid spherical coordinate
	Vec3 l = random_sphereical_vector(0.0, pi / 2.0);

	//transform this vector into original world coordinates.
	return (l.x() * tangent) + (l.y() * bitangent) + (l.z() * n);
}

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
	return v - 2*dot(v,n)*n;
}

inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
	auto cos_theta = std::fmin(dot(-uv, n), 1.0);
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
	Vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}

template<>
struct std::formatter<Vec3> {
	
	constexpr auto parse(std::format_parse_context& ctx) {
 		
		return ctx.begin();
	}

	auto format(const Vec3& obj, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "Vec3({}, {}, {})", obj.x(), obj.y(), obj.z());
	}

};

