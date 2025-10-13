#pragma once

#include "ray.hpp"


struct HitRecord {
	Point3D p;
	Vec3 normal;
	double t;
};

class Hittable {
public:
	virtual ~Hittable() = default;

	virtual bool hit(const Ray& r, double ray_tmin, double ray_tmax, HitRecord& rec) const = 0;
};
