#pragma once

#include "constants.hpp"
#include "hittable.hpp"
#include "texture.hpp"

class Material {
public:
	virtual ~Material() = default;

	virtual bool scatter(
		const Ray&, const HitRecord&, Vec3&, Ray& 
	) const {
		return false;
	}
};

class Lambertian : public Material {
public:
	Lambertian(const Vec3& albedo) : albedo(albedo) {}

	bool scatter(const Ray&, const HitRecord &rec, Vec3 &attentuation, Ray &scattered) const override {
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


class LambertianTexture : public Material {
public:
	LambertianTexture(std::shared_ptr<Texture> texture) : texture_(texture) {}
	
	bool scatter(const Ray&, const HitRecord &rec, Vec3 &attentuation, Ray &scattered) const override {
		auto scatter_direction = rec.normal + random_unit_vector();

		if (scatter_direction.near_zero())
		{
			scatter_direction = rec.normal;
		}

		scattered = Ray(rec.p, scatter_direction);
		auto albedo = texture_->sample(rec.uv.x(), rec.uv.y());
		attentuation = albedo.to_vec3();
		return true;
	}

private:
	std::shared_ptr<Texture> texture_;
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
		double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0;
		Vec3 direction;

		if (cannot_refract || reflectance(cos_theta, ri) > random_double()) 
			direction = reflect(unit_direction, rec.normal);
		else
			direction = refract(unit_direction, rec.normal, ri);

		scattered = Ray(rec.p, direction);
		return true;
	}
private:
	double wavefactor;

	static double reflectance(double cosine, double refraction_index) {
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0*r0;
		return r0 + (1-r0)*std::pow((1-cosine), 5);
	}
};

class Light : public Material {
public:

	bool scatter(const Ray&, const HitRecord&, Vec3 &attentuation, Ray&) const override 
	{
		attentuation = Vec3(1.0, 1.0, 1.0);
		return false;
	}


};
