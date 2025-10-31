#pragma once

#include "hittable.hpp"


class Sphere : public Hittable {
public:
	Sphere(const Point3D& center, double radius, std::shared_ptr<Material> mat) : center_(center), radius(std::fmax(0, radius)), mat(mat) {}

	bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		Vec3 oc = center_ - r.origin();
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
		Vec3 outward_normal = (rec.p - center_) / radius; //the vector from center to P, normalized.
		rec.set_face_normal(r, outward_normal);
		rec.mat = mat;

		//the outward_normal is also just the vector on the unit_sphere
		//Let's calculate the UV for a sphere...
		auto theta = std::acos(outward_normal.z());
		auto phi = std::atan2(outward_normal.y(), outward_normal.x());

		rec.uv = Vec2(phi / 2 * pi, theta / pi);
		return true;
	}

private:
	Point3D center_;
	double radius;
	std::shared_ptr<Material> mat;
};
