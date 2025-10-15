#pragma once

#include "constants.hpp"

class Interval {
public:
	double low, high;

	Interval() : low(-infinity), high(infinity) {}
	Interval(double low, double high) : low(low), high(high) {}

	bool contains(double val) const {
		return (val >= low && val <= high);
	}
	
	double size() const {
		return high - low;
	}
	
	bool surrounds(double x) const {
		return x > low && x < high;
	}

	double clamp(double x) const {
		if (x < low) return low;
		if (x > high) return high;
		return x;
	}
	
	static const Interval empty, universe;
	
};

const Interval Interval::empty = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval();
