#pragma once

#include <cmath>
#include <limits>
#include <random>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.14159265;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

//generates a random double between 0 and 1.0, [0.0, 1.0)
inline double random_double() {
	thread_local std::mt19937 generator;
	static thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
	return distribution(generator);
}

//generates a random double between [min, max)
inline double random_double(double min, double max) {
	return min + (max-min)*random_double();
}

