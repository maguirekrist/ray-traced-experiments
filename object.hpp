#pragma once


#include "hittable.hpp"
#include "vec.hpp"
#include <fstream>
#include <print>
#include <stdexcept>
#include <string>
#include <vector>

class Triangle : public Hittable {
public:
	
	Triangle() = delete;
	Triangle(Vec3 a, Vec3 b, Vec3 c) noexcept : f_{std::move(a), std::move(b), std::move(c)} {
		compute_normal();
	}

	bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override {
		//check if it hits the plane and get where on the plane the ray hit...
		
		auto denom = dot(r.direction(), n_);
		if (std::abs(denom) < 1e-12) return false;

		//plane origin can just be the first vertices for now...
		auto plane_origin = f_[0];
		auto t = dot(plane_origin - r.origin(), n_) / denom;

		if (ray_t.contains(t)) {
			//Ok, now we have t, meaning...
			auto world_pos = r.at(t); //we can get the world position of the ray on this plane created by the triangle.
			//we need to check now to see if x is really on the triangle
			bool is_outside = false;
			auto edge1 = f_[1] - f_[0];
			auto edge2 = f_[2] - f_[1];
			auto edge3 = f_[0] - f_[2];
			auto toP1 = world_pos - f_[0];
			auto toP2 = world_pos - f_[1];
			auto toP3 = world_pos - f_[2];

			if (dot(cross(edge1, toP1), n_) < 0) is_outside = true;
			if (dot(cross(edge2, toP2), n_) < 0) is_outside = true;
			if (dot(cross(edge3, toP3), n_) < 0) is_outside = true;
			if(is_outside) return false;

			rec.t = t;
			rec.p = world_pos;
			rec.set_face_normal(r, n_);

		        //TODO:	rec.uv
			return true;
		}
		return false;
	}
private:
	Vec3 f_[3]; //Vertices
	Vec3 n_; //normal
	//
	void compute_normal() {
		//we assume that all 3 vertices exist on the same "plane"
		//That means we can just pick two random vectors, and just take the cross product.
		n_ = cross(f_[1] - f_[0], f_[2] - f_[0]);
	}
};


class Object : public Hittable {
public: 

	explicit Object(std::vector<Triangle> faces, std::shared_ptr<Material> mat) : faces_(std::move(faces)), mat_(mat) {}

	bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override {
		//naive implementation just loop through all triangles to see what hits? 
		bool hit_anything = false;
		double closest = ray_t.high;

		HitRecord temp;
		for(const auto& t : faces_) {
			if (t.hit(r, Interval(ray_t.low, closest), temp)) {
				hit_anything = true;
				closest = temp.t;
				rec = temp;
				rec.mat = mat_;
			}
		}

		return hit_anything;
	}
private:
	std::vector<Triangle> faces_;
	std::shared_ptr<Material> mat_;
};


inline std::shared_ptr<Object> parse_obj(const std::string& path, std::shared_ptr<Material> mat, Point3D origin) {
	std::vector<Vec3> vertices;
	//std::vector<Vec3> normals;
	std::vector<Triangle> triangles;

	std::ifstream file(path);
	
	if (!file.is_open()) throw std::runtime_error("Unable to open file!");
	
	std::string line;
	while(std::getline(file, line)) {
		if (line[0] == 'v') {
			//Parse vertex
			double f[3];
			int counter = 0;
			int start = 2;
			for(size_t i = 2; i < line.length(); ++i) {
				if (line[i] == ' ') {
					auto slice = line.substr(start, i - start);
					f[counter] = std::stod(slice);
					++counter;
					start = i+1;
				}
			}

			//parse last
			auto slice = line.substr(start);
			f[counter] = std::stod(slice);

			vertices.push_back(Vec3(f[0], f[1], f[2]));
		} else if(line[0] == 'f') {
			//Parse face (this is a triangle)
			int f[3];
			int counter = 0;
			int start = 2;
			for (size_t i = 2; i < line.length(); ++i) {
				if (line[i] == ' ') {
					auto slice = line.substr(start, i - start);
					f[counter] = std::stoi(slice) - 1;
					++counter;
					start = i+1;
				}
			}

			auto slice = line.substr(start);
			f[counter] = std::stoi(slice) - 1;

			triangles.push_back(Triangle(vertices[f[0]] + origin, vertices[f[1]] + origin, vertices[f[2]] + origin));
		} else {
			continue;
		}	
	}

	return std::make_shared<Object>(std::move(triangles), mat);
}

