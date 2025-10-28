#pragma once

#include "material.hpp"

class Light : public Material {



	bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attentuation, Ray &scattered) const override;
};
