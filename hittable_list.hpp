#pragma once

#include <stdexcept>
#include <vector>
#include <memory>
#include "hittable.hpp"

class HittableList : public Hittable {
public:
	std::vector<std::shared_ptr<Hittable>> objects;


	HittableList() {}
	HittableList(std::shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }

	void add(std::shared_ptr<Hittable> object) {
		objects.push_back(object);
	}

	bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override {
		HitRecord temp_rec;
		bool hit_anything = false;
		auto closest_so_far = ray_t.high;
		for (const auto& object: objects)
		{
			if(object->hit(r, Interval(ray_t.low, closest_so_far), temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}

	Point3D center() const override {
		throw std::runtime_error("Cannot call center on world!");
	}
};
