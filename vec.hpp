#pragma once

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


template<>
struct std::formatter<Vec3> {
	
	constexpr auto parse(std::format_parse_context& ctx) {
 		
		return ctx.begin();
	}

	auto format(const Vec3& obj, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "Vec3({}, {}, {})", obj.x(), obj.y(), obj.z());
	}

};

