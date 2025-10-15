#pragma once

#include <print>

#include "ray.hpp"
#include "hittable.hpp"
#include "constants.hpp"

struct Color {
	int r, g, b;	

	Color() {}
	Color(int r, int g, int b) : r(r), g(g), b(b) {}

	Color(const Vec3& vector) {
		auto unit_v = unit_vector(vector);
		
		static const Interval intensity(0.000, 0.999);
		r = 256 * intensity.clamp(unit_v.x());
		g = 256 * intensity.clamp(unit_v.y());
		b = 256 * intensity.clamp(unit_v.z());
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
	int samples_per_pixel = 100;	

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
					pixel_color += ray_color(r, world);
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

	void initialize() {
		image_height = int(image_width / aspect_ratio);

		auto focal_length = 1.0;
		auto viewport_height = 2.0;
		
		auto viewport_width = viewport_height * (double(image_width)/image_height);
		center = Point3D(0, 0, 0);

		std::println("Viewport width: {}, height: {}", viewport_width, viewport_height);
		auto viewport_u = Vec3(viewport_width, 0, 0);
		auto viewport_v = Vec3(0, -viewport_height, 0);

		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		auto viewport_upper_left = center - Vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		std::println("Viewport upper left is: {}", viewport_upper_left);
		std::println("Pixel 00 location is: {}", pixel00_loc);

		pixel_samples_scale = 1.0 / samples_per_pixel;
	}

	Vec3 ray_color(const Ray& r, const Hittable& world) 
	{
		HitRecord rec;
		if (world.hit(r, Interval(0, infinity), rec))
		{
			return 0.5 * (rec.normal + Vec3(1,1,1));
		}
		auto unit_direction = unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		//lerp - from 0 to a, from value to value. 
		return (1.0 - a)*Vec3(1.0, 1.0, 1.0) + a *Vec3(0.5, 0.7, 1.0);	
	}

	Ray get_ray(int i, int j) {
		//generates a ray for pixel i, and j
		//generate a random jitter
		double jitter_i = random_double(-1.0, 1.0);
		double jitter_j = random_double(-1.0, 1.0);
		auto pixel_center = pixel00_loc + ((i + jitter_i) * pixel_delta_u) + ((j + jitter_j) * pixel_delta_v);

		auto ray_direction = pixel_center - center; 
		Ray r(center, ray_direction);

		return r;
	}
	
	void write_color(std::ostream& out, const Vec3& color)
	{
		Color c(color);
		out << c << ' ' << std::endl;
	}
}; 


