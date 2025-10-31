#pragma once

#include "hittable.hpp"
#include "vec.hpp"

class Plane : public Hittable {
public:

	explicit Plane(Point3D point, Vec3 normal, std::shared_ptr<Material> mat) : p_(std::move(point)), n_(std::move(normal)), mat_(mat) {
		Vec3 a = (std::abs(n_.x()) > 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
		t1_ = unit_vector(cross(a, n_));
		t2_ = cross(n_, t1_);
	};

	bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override {
		//calculate T
		// t = (r0 - p) * n / Rn_ * n_
		auto bottom = dot(r.direction(), n_);
		if (bottom == 0.0) {
			return false;
		}
		auto t = dot((r.origin() - p_), n_) / bottom; 

		if (ray_t.surrounds(t)) {
			rec.t = t;
			rec.p = r.at(t);
			rec.normal = unit_vector(rec.p);
			rec.front_face = dot(r.direction(), n_) < 0;
			rec.mat = mat_;
			rec.uv = Vec2(dot(rec.p - p_, t1_), dot(rec.p - p_, t2_));
			return true;
		}

		return false;
	}
private:
	Point3D p_;
	Vec3 n_;
	std::shared_ptr<Material> mat_;

	Vec3 t1_;
	Vec3 t2_;
};
