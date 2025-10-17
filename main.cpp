#include <memory>
#include <optional>
#include <print>
#include <random>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "material.hpp"
#include "ray.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "camera.hpp"

double hit_sphere(const Point3D& center, double radius, const Ray& r) {
	Vec3 oc = center - r.origin();
	auto a = dot(r.direction(), r.direction());
	auto b = -2.0 * dot(r.direction(), oc);
	auto c = dot(oc, oc) - radius*radius;
	auto descriminant = b*b - 4*a*c;

	if (descriminant < 0)
	{
		return -1;
	} else {
		return (-b - std::sqrt(descriminant)) / (2.0*a);
	}
}


void generate_test_ppm(int rows, int cols)
{
	std::random_device rnd;
	std::uniform_int_distribution<int> dist(0, 255);
	std::vector<Color> colors(rows * cols);

	for(int r = 0; r < rows; r++)
	{
		for(int c = 0; c < cols; c++)
		{
			colors[r * cols + c] = Color(dist(rnd), dist(rnd), dist(rnd));
		}
	}

	//write buffer
	std::ofstream file;
	file.open("example.ppm", std::ios::trunc);
	
	file << "P3" << std::endl;

	file << rows << " " << cols << '\n';
	
	file << 255 << '\n';
	
	for(auto color : colors)
	{
		file << color << '\n';
	}

	file.flush();
	file.close();
}

constexpr auto aspect_ratio = 16.0 / 9.0;
constexpr auto image_width = 900;
constexpr auto image_height = static_cast<int>(image_width / aspect_ratio);

struct Config { 
	int samples_per_pixel = 100;
	int image_width = 600;
};

Config parse_args(int arg_count, char *args[])
{
	Config config;
	if (arg_count >= 2)
	{
		std::string imageWidth = args[1];
		config.image_width = std::stoi(imageWidth);
	}
	if (arg_count >= 3)
	{
		std::string samplesPerPixel = args[2];
		config.samples_per_pixel = std::stoi(samplesPerPixel);
	}
     	return config;
}

int main(int argc, char *argv[]) {
	
	auto config = parse_args(argc, argv);	

	//world
	std::ofstream file;
	file.open("example.ppm", std::ios::trunc);

	HittableList world;

	auto material_ground = std::make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5));
	auto material_red = std::make_shared<Lambertian>(Vec3(1.0, 0.0, 0.0));
	auto material_left = std::make_shared<Metal>(Vec3(0.8, 0.8, 0.8), 0.0);
	auto material_right = std::make_shared<Metal>(Vec3(0.8, 0.6, 0.2), 0.5);
	auto material_reflective = std::make_shared<Dielectric>(1.50);	

	world.add(std::make_shared<Sphere>(Point3D(0,0,-1), 0.5, material_center));	
	world.add(std::make_shared<Sphere>(Point3D(0,-100.5, -1), 100, material_ground));
	world.add(std::make_shared<Sphere>(Point3D(-1.0, 0.0, -1), 0.5, material_left));
	world.add(std::make_shared<Sphere>(Point3D(1.0, 0.0, -1), 0.5, material_reflective));
	world.add(std::make_shared<Sphere>(Point3D(-2, 2, 3), 0.5, material_red));

	Camera camera;

	camera.aspect_ratio = aspect_ratio;
	camera.image_width = config.image_width;
	camera.samples_per_pixel = config.samples_per_pixel;
	camera.vfov = 20.0;
	camera.lookfrom = Point3D(-2, 2, 1);
	camera.lookat = Point3D(0, 0, -1);
	camera.vup = Vec3(0, 1, 0);

	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.4;

	std::println("Generating image of width: {}, and height: {}", camera.image_width, image_height);
	camera.render(file, world);
	file.close();
	return 0;
}

