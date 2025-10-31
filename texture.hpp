#pragma once

#include "color.hpp"
#include <print>

class Texture {
public:
	virtual ~Texture() = default;

	virtual Color sample(double u, double v) const = 0;
};

class TestTexture : public Texture {
public:

	explicit TestTexture(Color primary, int tiles_delta) : primary_(primary), tiles_u_(tiles_delta), tiles_v_(tiles_delta) {}

	// Color sample(int x, int y) const override {
	// 	//for like every other "square" we want to color a specific way.
	// 	auto in_texture_x = x % (square_size_);
	// 	auto in_texture_y = y % (square_size_);
	//
	// 	if (in_texture_x != in_texture_y) {
	// 		//dampen the sqaure color
	// 		return Color(primary_.r * 0.4, primary_.g * 0.4, primary_.b * 0.4);
	// 	}
	//
	// 	return primary_;
	// }
	//
	
	Color sample(double u, double v) const override {

		auto fract = [](double x){ return x - std::floor(x); };

		auto u_frac = fract(u);
		auto v_frac = fract(v);

		int ix = static_cast<int>(std::floor(u_frac * tiles_u_));
		int iy = static_cast<int>(std::floor(v_frac * tiles_v_));
		
		bool dark = ((ix + iy) & 1) != 0;
		if (dark) return Color(primary_.r * 0.4f, primary_.g * 0.4f, primary_.b * 0.4f);
		return primary_;
	}

private:
	Color primary_;
	int tiles_u_, tiles_v_;
//	int square_size_ = 2;
};
