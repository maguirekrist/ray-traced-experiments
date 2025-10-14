#pragma once

#include <cmath>
#include <limits>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.14159265;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}



