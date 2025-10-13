#pragma once

#include "hittable.hpp"


class Sphere : public Hittable {
public:
	Sphere(const Point3D& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

	bool hit(const Ray& r, double ray_tmin, double ray_tmax, HitRecord& rec) const override {
		return true;
	}

private:
	Point3D center;
	double radius;
};
