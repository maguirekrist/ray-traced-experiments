#pragma once

#include "hittable.hpp"


class Sphere : public Hittable {
public:
	Sphere(const Point3D& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

	bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		Vec3 oc = center - r.origin();
		auto a = r.direction().length_squared();
		auto h = dot(r.direction(), oc);
		auto c = oc.length_squared() - radius*radius;

		auto descriminant = h*h - a*c;
		if (descriminant < 0)
		{
			return false;
		}

		auto sqrtd = std::sqrt(descriminant);

		auto root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) 
		{
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root)) 
				return false;
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		rec.normal = (rec.p - center) / radius;

		return true;
	}
private:
	Point3D center;
	double radius;
};
