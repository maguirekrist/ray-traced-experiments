#pragma once

#include "hittable.hpp"
#include "vec.hpp"
class Material {
public:
	virtual ~Material() = default;

	virtual bool scatter(
		const Ray& r_in, const HitRecord& rec, Vec3& attentuation, Ray& scattered
	) const {
		return false;
	}
};

class Lambertian : public Material {
public:
	Lambertian(const Vec3& albedo) : albedo(albedo) {}

	bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attentuation, Ray &scattered) const override {
		auto scatter_direction = rec.normal + random_unit_vector();

		if (scatter_direction.near_zero())
		{
			scatter_direction = rec.normal;
		}

		scattered = Ray(rec.p, scatter_direction);
		attentuation = albedo;
		return true;
	}

private:
	//albedo is latin for "whiteness"
	//used to define a form of "fractional reflectance"
	Vec3 albedo;
};

class Metal : public Material {
public:
	Metal(const Vec3& albedo) : albedo(albedo) {};

	bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attentuation, Ray &scattered) const override {
		Vec3 reflected = reflect(r_in.direction(), rec.normal);
		scattered = Ray(rec.p, reflected);
		attentuation = albedo;
		return true;
	}

private:
	Vec3 albedo;
};

