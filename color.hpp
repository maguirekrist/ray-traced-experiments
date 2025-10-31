#pragma once

#include "interval.hpp"
#include "vec.hpp"

inline double linear_to_gamma(double linear_component)
{
	if (linear_component > 0)
		return std::sqrt(linear_component);

	return 0.0;
}


struct Color {
	int r, g, b;	

	Color() {}
	Color(int r, int g, int b) : r(r), g(g), b(b) {}

	Color(const Vec3& vector) {
		static const Interval intensity(0.000, 0.999);

		auto rbyte = linear_to_gamma(vector.x());
		auto gbyte = linear_to_gamma(vector.y());
		auto bbyte = linear_to_gamma(vector.z());


		r = int(256 * intensity.clamp(rbyte));
		g = int(256 * intensity.clamp(gbyte));
		b = int(256 * intensity.clamp(bbyte));
	}

	
	Vec3 to_vec3() const {
		return Vec3(r / 256.0, g / 256.0, b / 256.0);
	}

	explicit operator Vec3() const { return to_vec3(); }
};

inline std::ostream& operator<<(std::ostream& ostream, const Color& color)
{
	ostream << color.r << " " << color.g << " " << color.b;
	return ostream;
}
