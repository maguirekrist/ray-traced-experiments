#pragma once

#include <memory>
#include "interval.hpp"
#include "ray.hpp"

class Material;

struct HitRecord {
	Point3D p; //point of contact
	Vec3 normal; //surface normal
	std::shared_ptr<Material> mat; //hit material
	double t; //the t value that solved the hit equation.
	bool front_face; //are we facing the front?
	Vec2 uv;


	void set_face_normal(const Ray& r, const Vec3& outward_normal) {
		// sets the hit record normal vector;
		// NOTE: the parameter outward_normal is assumed to have unit length;
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class Hittable {
public:
	virtual ~Hittable() = default;

	virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;
};
