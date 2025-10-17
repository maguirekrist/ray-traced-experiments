#pragma once

#include <cmath>
#include <print>

#include "ray.hpp"
#include "hittable.hpp"
#include "constants.hpp"
#include "vec.hpp"
#include "material.hpp"

inline double linear_to_gamma(double linear_component)
{
	if (linear_component > 0)
		return std::sqrt(linear_component);

	return 0.0;
}

struct Color {
	int r, g, b;	

	Color() {}
	Color(int r, int g, int b) : r(r), g(g), b(b) {}

	Color(const Vec3& vector) {
		static const Interval intensity(0.000, 0.999);

		auto rbyte = linear_to_gamma(vector.x());
		auto gbyte = linear_to_gamma(vector.y());
		auto bbyte = linear_to_gamma(vector.z());


		r = int(256 * intensity.clamp(rbyte));
		g = int(256 * intensity.clamp(gbyte));
		b = int(256 * intensity.clamp(bbyte));
	}

};

inline std::ostream& operator<<(std::ostream& ostream, const Color& color)
{
	ostream << color.r << " " << color.g << " " << color.b;
	return ostream;
}

class Camera {
public:
	double aspect_ratio = 	1.0;
	int image_width = 400;
	int samples_per_pixel = 10;	
	int max_depth = 10;
	double vfov = 90.0;

	Point3D lookfrom = Point3D(0.0, 0.0, 0.0); // Point camera is looking from
	Point3D lookat = Point3D(0.0, 0.0, -1); // Point camera is looking at
	Vec3 vup = Vec3(0.0, 1.0, 0.0); ///Camera-relative up direction
	
	double defocus_angle = 0; // Variaton angle of rays through each pixel.
	double focus_dist = 10; // Distance from camera lookfrom point to plane of perfect focus

	void render(std::ostream& out, const Hittable& world)
	{
		initialize();

		out << "P3" << '\n'; 
		out << image_width << " " << image_height << '\n';

		out << 255 << '\n'; 
		
		for (auto j = 0; j < image_height; j++)
		{
			for(auto i = 0; i < image_width; i++)
			{
				Vec3 pixel_color(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; sample++)
				{
					Ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}

			//	auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
			//	auto ray_direction = pixel_center - center; 
			//	Ray r(center, ray_direction);

			//	Vec3 color = ray_color(r, world);
				write_color(out, pixel_samples_scale * pixel_color);
			}
		}

		out.flush();

	}
private:
	int image_height;
	Point3D center;
	Point3D pixel00_loc;
	Vec3 pixel_delta_u;
	Vec3 pixel_delta_v;
	double pixel_samples_scale;

	Vec3 u, v, w; //Camera frame basis vectors.
	Vec3 defocus_disk_u;   	//Defocus disk horizontal radius
	Vec3 defocus_disk_v;	//Defocus disk vertical radius

	void initialize() {
		image_height = int(image_width / aspect_ratio);

		// auto focal_length = (lookfrom - lookat).length();

		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta/2);

		auto viewport_height = 2 * h * focus_dist;		
		auto viewport_width = viewport_height * (double(image_width)/image_height);
		center = lookfrom; 

		w = unit_vector(lookfrom - lookat); //Vector from look-at to lookfrom
		u = unit_vector(cross(vup, w)); //vector orthogonal to vup and w.
		v = cross(w, u); //a vector orthogonal to w and u.

		std::println("Viewport width: {}, height: {}", viewport_width, viewport_height);
		auto viewport_u = viewport_width * u;
		auto viewport_v = viewport_height * -v;

		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;

		std::println("Viewport upper left is: {}", viewport_upper_left);
		std::println("Pixel 00 location is: {}", pixel00_loc);

		pixel_samples_scale = 1.0 / samples_per_pixel;
	}

	Vec3 ray_color(const Ray& r, int depth, const Hittable& world) 
	{
		if (depth <= 0)
		{
			return Vec3(0, 0, 0);
		}

		HitRecord rec;
		if (world.hit(r, Interval(0.001, infinity), rec))
		{
			Ray scattered;
			Vec3 attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered))
				return attenuation * ray_color(scattered, depth-1, world);
			return Vec3(0, 0, 0);
		}

		auto unit_direction = unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		//lerp - from 0 to a, from value to value. 
		return (1.0 - a)*Vec3(1.0, 1.0, 1.0) + a *Vec3(0.5, 0.7, 1.0);	
	}

	Ray get_ray(int i, int j) {
		//generates a ray for pixel i, and j
		//generate a random jitter
		double jitter_i = random_double(-1.0, 1.0) / 2.0;
		double jitter_j = random_double(-1.0, 1.0) / 2.0;
		auto pixel_center = pixel00_loc + ((i + jitter_i) * pixel_delta_u) + ((j + jitter_j) * pixel_delta_v);

		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_center - ray_origin; 
		Ray r(center, ray_direction);

		return r;
	}

	
	Point3D defocus_disk_sample() const {
		auto p = random_in_unit_disk();
		return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
	}
	
	void write_color(std::ostream& out, const Vec3& color)
	{
		Color c(color);
		out << c << ' ' << std::endl;
	}
}; 


