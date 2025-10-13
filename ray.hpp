#pragma once

#include "vec.hpp"

class Ray {
public:
	Ray() {}

	Ray(const Point3D& org, const Vec3& direction) : orig(org), dir(direction) {}
	

	const Point3D& origin() const { return orig; }
	const Vec3& direction() const { return dir; }

	//calculates the point3D(Vec3) that is at the delta T on the ray line.
	Vec3 at(double t) const {
		return orig + t*dir;
	}
private:
	Point3D orig;
	Vec3 dir;
};
