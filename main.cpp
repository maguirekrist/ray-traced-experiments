#include <memory>
#include <print>
#include <random>
#include <vector>
#include <iostream>
#include <fstream>

#include "constants.hpp"
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
	file.open("example.ppm");
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
constexpr auto image_width = 400;
constexpr auto image_height = static_cast<int>(image_width / aspect_ratio);

int main() {
	std::println("Generating image of width: {}, and height: {}", image_width, image_height);

	//world
	std::ofstream file;
	file.open("example.ppm");

	HittableList world;

	world.add(std::make_shared<Sphere>(Point3D(0,0,-1), 0.5));	
	world.add(std::make_shared<Sphere>(Point3D(0,-100.5, -1), 100));

	Camera camera;

	camera.aspect_ratio = aspect_ratio;
	camera.image_width = image_width;

	camera.render(file, world);
	file.close();
	return 0;
}

