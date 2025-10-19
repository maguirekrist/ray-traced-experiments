#include <memory>
#include <optional>
#include <print>
#include <random>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "constants.hpp"
#include "material.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "lib/cfg/config.hpp"

constexpr auto aspect_ratio = 16.0 / 9.0;

struct Config { 
	int samples_per_pixel = 100;
	int image_width = 600;
	double vfov = 90.0;
	double focus_dist = 2.0;
	double defocus_angle = 10.0;
	int maximum_depth = 10;
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

Config parse_ini(const std::string path) {
	Config config;
	std::expected<cfg::Config, std::string> t_cfg = cfg::parse_file("init.ini");
	if (t_cfg.has_value())
	{
		config.samples_per_pixel = t_cfg->get_value_or("samples_per_pixel", config.samples_per_pixel); 
		config.image_width = t_cfg->get_value_or("image_width", config.image_width);
		config.vfov = t_cfg->get_value_or("vfov", config.vfov);
		config.focus_dist = t_cfg->get_value_or("focus_dst", config.focus_dist);
		config.defocus_angle = t_cfg->get_value_or("defoucs_angle", config.defocus_angle);
		config.maximum_depth = t_cfg->get_value_or("maximum_depth", config.maximum_depth);
	} else {
		std::string err_str = t_cfg.error();
		std::println("{}", err_str);
	}

	return config;
}

HittableList gen_world(int objCount = 10) {
	HittableList world;

	auto ground_material = std::make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5)); 
	world.add(std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material));

	for (int a = -objCount; a < objCount; a++)
	{
		for (int b = -objCount; b < objCount; b++) {
			auto choose_mat = random_double();
			Point3D center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

			if ((center - Point3D(4, 0.2, 0)).length() > 0.9) {
				std::shared_ptr<Material> sphere_material;

				if (choose_mat < 0.8) {
					//diffuse
					auto albedo = Vec3::random() * Vec3::random();
					sphere_material = std::make_shared<Lambertian>(albedo);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				} else if (choose_mat < 0.95) {
					//metal
					auto albedo = Vec3::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);

					sphere_material = std::make_shared<Metal>(albedo, fuzz);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				} else {
					//glass
					sphere_material = std::make_shared<Dielectric>(1.5);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto glass_mat = std::make_shared<Dielectric>(1.5);
	auto diffuse_mat = std::make_shared<Lambertian>(Vec3(0.4, 0.2, 0.1));
	auto metal_mat = std::make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0);
	
	world.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, glass_mat));
	world.add(std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, diffuse_mat));
	world.add(std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, metal_mat));
	return world;
}

HittableList gen_test_scene() {
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

	return world;
}

int main(int argc, char *argv[]) {

	//if we want to parse by args.
//	auto config = parse_args(argc, argv);	
	auto config = parse_ini("init.ini");
	std::ofstream file;
	file.open("example.ppm", std::ios::trunc);

	HittableList world = gen_world(15);

	Camera camera;

	camera.aspect_ratio = aspect_ratio;
	camera.image_width = config.image_width;
	camera.samples_per_pixel = config.samples_per_pixel;
	camera.vfov = config.vfov;
	camera.max_depth = config.maximum_depth;
	camera.lookfrom = Point3D(13, 2, 3);
	camera.lookat = Point3D(0, 0, 0);
	camera.vup = Vec3(0, 1, 0);

	//not using focus blur right now
	camera.defocus_angle = config.defocus_angle;
	camera.focus_dist = config.focus_dist;

	camera.render(file, world);
	file.close();
	return 0;
}

