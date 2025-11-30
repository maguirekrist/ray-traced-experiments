#pragma once

#include <cmath>
#include <print>
#include <vector>
#include <cassert>
#include <chrono>
#include "ray.hpp"
#include "hittable.hpp"
#include "constants.hpp"
#include "material.hpp"
#include "vec.hpp"
#include "thread_pool.hpp"
#include "lib/tui/tui.hpp"
#include "color.hpp"




struct Kernel {
	int width = 0;
	int height = 0;
	int startW = 0;
	int startH = 0;
	std::vector<Vec3> colors;
};


class Camera {
public:
	double aspect_ratio = 16.0 / 9.0;
	int image_width = 400;
	int samples_per_pixel = 10;	
	int max_depth = 10;
	double vfov = 90.0;

	Point3D lookfrom = Point3D(0.0, 0.0, 0.0); // Point camera is looking from
	Point3D lookat = Point3D(0.0, 0.0, -1); // Point camera is looking at
	Vec3 vup = Vec3(0.0, 1.0, 0.0); ///Camera-relative up direction
	
	double defocus_angle = 0; // Variaton angle of rays through each pixel.
	double focus_dist = 10; // Distance from camera lookfrom point to plane of perfect focus
	

	Camera() {}
	explicit Camera(std::vector<Vec3> lights) : light_sources(lights) {  }
	

	void render(std::ostream& out, const Hittable& world)
	{
		initialize();

		out << "P3" << '\n'; 
		out << image_width << " " << image_height << '\n';
		out << 255 << '\n'; 
		std::println("Generating image of width: {}, height: {}", image_width, image_height);	


		const int tiles_x = 16;
		const int tiles_y = 16;

		const int tile_w = (image_width + tiles_x - 1) / tiles_x;
		const int tile_h = (image_height + tiles_y - 1) / tiles_y;
		//break the image into 16 by 16 sections.


		std::println("Spawning {} workers", tiles_x * tiles_y);
		std::vector<Vec3> framebuffer(image_width * image_height);

		tui::LoadingIndicator loader(tiles_x * tiles_y);
		auto start = std::chrono::steady_clock::now();

		{
			ThreadPool pool;
			for(int ty = 0; ty < tiles_y; ty++)
			{
				for (int tx = 0; tx < tiles_x; tx++)
				{
					const int x0 = tx*tile_w;
					const int x1 = std::min(x0 + tile_w, image_width);
					const int y0 = ty*tile_h;
					const int y1 = std::min(y0 + tile_h, image_height);
					
					pool.execute([&, x0, x1, y0, y1] {
						render_tile(world, x0, y0, x1, y1, framebuffer);
						++loader;
					});
				}
			}
		}
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;

		std::cout.flush();
		
		std::println("\nWork completed in {} seconds!", elapsed_seconds.count());

		for(int y = 0; y < image_height; ++y)
		{
			for(int x = 0; x < image_width; ++x)
			{
				write_color(out, framebuffer[y * image_width + x]);
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
//	Vec3 defocus_disk_u;   	//Defocus disk horizontal radius
//	Vec3 defocus_disk_v;	//Defocus disk vertical radius
	std::vector<Vec3> light_sources;

	void initialize() {
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;
		pixel_samples_scale = 1.0 / samples_per_pixel;
		center = lookfrom; 

		auto focal_length = (lookfrom - lookat).length();
		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta/2);

		auto viewport_height = 2 * h * focal_length;		
		auto viewport_width = viewport_height * (double(image_width)/image_height);

		w = unit_vector(lookfrom - lookat); //Vector from look-at to lookfrom
		u = unit_vector(cross(vup, w)); //vector orthogonal to vup and w.
		v = cross(w, u); //a vector orthogonal to w and u.

		std::println("Viewport width: {}, height: {}", viewport_width, viewport_height);
		auto viewport_u = viewport_width * u;
		auto viewport_v = viewport_height * -v;

		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		auto viewport_upper_left = center - (focal_length * w) - viewport_u/2 - viewport_v/2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

//		auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2.0));
//		defocus_disk_u = u * defocus_radius;
//		defocus_disk_v = v * defocus_radius;

		std::println("Viewport U: {}", viewport_u);
		std::println("Viewport V: {}", viewport_v);
		std::println("Viewport upper left is: {}", viewport_upper_left);
		std::println("Pixel 00 location is: {}", pixel00_loc);
		std::println("Samples per pixel: {}", samples_per_pixel);
		std::println("vfov: {}", vfov);
		std::println("focus_dist: {}", focus_dist);

	}


	void render_tile(const Hittable& world, int startWidth, int startHeight, int width, int height, std::vector<Vec3>& fb)
	{
		for(int y = startHeight; y < height; y++) {
			for(int x = startWidth; x < width; x++)
			{
				Vec3 pixel_color(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; sample++)
				{
					Ray r = get_ray(x, y);
					pixel_color += ray_color(r, max_depth, world);
				}

				fb[y * image_width + x] = pixel_samples_scale * pixel_color;

			}
		}
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
			//We've hit something in the world
			Vec3 output(0.0, 0.0, 0.0);
			Ray scattered;
			Vec3 attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered))
				output = attenuation * ray_color(scattered, depth-1, world);
					
			//We need to now attenuate the reflected light if there exists a shadow on this point.
			//First, we should create a ray that points to a light source whose origin is the hit point.
			for(auto& light_center : light_sources) {
				//we need to get the Direction vector, i.e. a normal from the point to the light center
				auto difference_vector = light_center - rec.p; //the vector from point P to light
				Vec3 dir = unit_vector(difference_vector); 
				Ray ray(rec.p, dir);
				//Ok, we need to now check if this shadow ray intersects with anything in the world!
				HitRecord shadow_hit;
				if (world.hit(ray, Interval(0.001, infinity), shadow_hit)) {
					//we hit something! we should dampen the output color now I guess?
					output *= 0.4;
				}
			}

			return output;
		}

		auto unit_direction = unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		//lerp - from 0 to a, from value to value. 
		return (1.0 - a)*Vec3(1.0, 1.0, 1.0) + a *Vec3(0.5, 0.7, 1.0);	
	}

	Ray get_ray(int i, int j) {
		//generates a ray for pixel i, and j
		//generate a random jitter - small random offset within the pixel
		double jitter_i = random_double(-0.5, 0.5);
		double jitter_j = random_double(-0.5, 0.5);
		auto pixel_center = pixel00_loc + ((i + jitter_i) * pixel_delta_u) + ((j + jitter_j) * pixel_delta_v);

		//auto ray_origin = (defocus_angle <= 0.1) ? center : defocus_disk_sample();
		auto ray_origin = center;
		auto ray_direction = pixel_center - ray_origin; 
		Ray r(ray_origin, ray_direction);

		return r;
	}

	//
	// Point3D defocus_disk_sample() const {
	// 	auto p = random_in_unit_disk();
	// 	return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
	// }
	
	void write_color(std::ostream& out, const Vec3& color)
	{
		Color c(color);
		out << c << ' ' << std::endl;
	}
	
	void write_kernel(std::ostream& out, const Kernel& kernel)
	{
		for(const auto& color : kernel.colors) {
			write_color(out, color);
		}	
	}
}; 


