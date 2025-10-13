#include <print>
#include <random>
#include <vector>
#include <iostream>
#include <fstream>

#include "ray.hpp"

struct Color {
	int r, g, b;	

	Color() {}
	Color(int r, int g, int b) : r(r), g(g), b(b) {}

	Color(const Vec3& vector) {
		auto unit_v = unit_vector(vector);

		r = 255 * unit_v.x();
		g = 255 * unit_v.y();
		b = 255 * unit_v.z();
	}
};

std::ostream& operator<<(std::ostream& ostream, const Color& color)
{
	ostream << color.r << " " << color.g << " " << color.b;
	return ostream;
}

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

Vec3 ray_color(const Ray& r) 
{
	auto t = hit_sphere(Point3D(0, 0, -1), 0.5, r);;
	if (t > 0.0)
	{
		Vec3 N = unit_vector(r.at(t) - Vec3(0, 0, -1));
		return 0.5 * Vec3(N.x()+1, N.y()+1, N.z()+1);
	}
	auto unit_direction = unit_vector(r.direction());
	auto a = 0.5 * (unit_direction.y() + 1.0);
	//lerp - from 0 to a, from value to value. 
	return (1.0 - a)*Vec3(1.0, 1.0, 1.0) + a *Vec3(0.5, 0.7, 1.0);	
}

void write_color(std::ostream& out, const Vec3& color)
{
	Color c(color);
	out << c << ' ' << std::endl;
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

	//Camera
	auto focal_length = 1.0;
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (double(image_width)/image_height);
	auto camera_center = Point3D(0, 0, 0);

	std::println("Viewport width: {}, height: {}", viewport_width, viewport_height);
	auto viewport_u = Vec3(viewport_width, 0, 0);
	auto viewport_v = Vec3(0, -viewport_height, 0);

	auto pixel_delta_u = viewport_u / image_width;
	auto pixel_delta_v = viewport_v / image_height;

	auto viewport_upper_left = camera_center - Vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
	auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

//	..generate_test_ppm(image_width, image_height);
	std::println("Viewport upper left is: {}", viewport_upper_left);
	std::println("Pixel 00 location is: {}", pixel00_loc);

	std::ofstream file;
	file.open("example.ppm");
	file << "P3" << '\n'; 
	file << image_width << " " << image_height << '\n';
	
	file << 255 << '\n'; 
	

	for (auto j = 0; j < image_height; j++)
	{
		for(auto i = 0; i < image_width; i++)
		{
			auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
			auto ray_direction = pixel_center - camera_center; 
			Ray r(camera_center, ray_direction);
			
			Vec3 color = ray_color(r);
			write_color(file, color);
		}
	}

	file.flush();
	file.close();

	return 0;
}

