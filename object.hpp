#pragma once


#include "hittable.hpp"
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

class Triangle : public Hittable {
public:
	explicit Triangle(const Vec3 (&f)[3]) noexcept {
		std::copy(std::begin(f), std::end(f), std::begin(f_));
	}
	explicit Triangle(Vec3 a, Vec3 b, Vec3 c) noexcept : f_{std::move(a), std::move(b), std::move(c)} {}

	bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override {

		return false;
	}
private:
	Vec3 f_[3];
};


class Object : public Hittable {
public: 

	explicit Object(std::vector<Triangle> faces) : faces_(std::move(faces)) {}

	bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override {
		//naive implementation just loop through all triangles to see what hits? 
		
		for(const auto& t : faces_) {
			if (t.hit(r, ray_t, rec)) {
				return true;
			}
		}

		return false;
	}
private:
	std::vector<Triangle> faces_;
};


inline Object parse_obj(const std::string& path) {
	std::vector<Vec3> vertices;
	//std::vector<Vec3> normals;
	std::vector<Triangle> trangles;

	std::ifstream file(path);
	
	if (!file.is_open()) throw std::runtime_error("Unable to open file!");
	
	std::string line;
	while(std::getline(file, line)) {
		if (line[0] == 'v') {
			//Parse vertex
			double f[3];
			int counter = 0;
			int start = 1;
			for(size_t i = 1; i < line.length(); ++i) {
				if (line[i] == ' ' || line[i] == '\n') {
					auto slice = line.substr(start, i - start);
					f[counter] = std::stod(slice);
					++counter;
					start = i+1;
				}
			}

			vertices.push_back(Vec3(f[0], f[1], f[2]));
		} else if(line[0] == 'f') {
			//Parse face (this is a triangle)
			int f[3];
			int counter = 0;
			int start = 1;
			for (size_t i = 1; i < line.length(); ++i) {
				if (line[i] == ' ' || line[i] == '\n') {
					auto slice = line.substr(start, i - start);
					f[counter] = std::stoi(slice) - 1;
					++counter;
					start = i+1;
				}
			}
			trangles.push_back(Triangle(vertices[f[0]], vertices[f[1]], vertices[f[2]]));
		} else {
			continue;
		}	
	}

	return Object(std::move(trangles));
}

