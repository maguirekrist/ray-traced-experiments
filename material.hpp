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
	Metal(const Vec3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {};

	bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attentuation, Ray &scattered) const override {
		Vec3 reflected = reflect(r_in.direction(), rec.normal);
		reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
		scattered = Ray(rec.p, reflected);
		attentuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

private:
	Vec3 albedo;
	double fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(double wavefactor) : wavefactor(wavefactor) {}

	bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attentuation, Ray &scattered) const override
	{
//		return false;
		attentuation = Vec3(1.0, 1.0, 1.0);	
		double ri = rec.front_face ? (1.0/wavefactor) : wavefactor;

		Vec3 unit_direction = unit_vector(r_in.direction());
		Vec3 refracted = refract(unit_direction, rec.normal, ri);

		scattered = Ray(rec.p, refracted);
		return true;
	}
private:
	double wavefactor;
};
