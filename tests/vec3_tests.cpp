#include <random>
#define CATCH_CONFIG_MAIN

#include "../third_party/catch2/catch_amalgamated.hpp"
#include "../vec.hpp"

TEST_CASE("SSE Vec result") {

	Vec3 v_A(5.0, 2.0, 1.0); 
	Vec3 v_B(1.0, 1.0, 0.0);

	double example = dot(v_A, v_B);
	double sse_example = dot_sse(v_A, v_B);

	REQUIRE(example == sse_example);
}


static double sum_dot_scalar(const std::vector<Vec3>& A, const std::vector<Vec3>& B) {
	double acc = 0.0;
	for(size_t i = 0; i < A.size(); ++i) acc += dot(A[i], B[i]);
	return acc;
}

static double sum_dot_avx(const std::vector<Vec3>& A, const std::vector<Vec3>& B) {
	double acc = 0.0;
	for(size_t i = 0; i < A.size(); ++i) acc += dot_sse(A[i], B[i]);
	return acc;
}

TEST_CASE("Vec Dot Benchmarks") {

	
	constexpr size_t N = 1 << 20;
	std::vector<Vec3> A(N), B(N);

	std::mt19937_64 rng(12345);
	std::uniform_real_distribution<double> dist(-1.0, 1.0);

	for(size_t i = 0; i < N; ++i) {
		A[i] = Vec3(dist(rng), dist(rng), dist(rng));
		B[i] = Vec3(dist(rng), dist(rng), dist(rng));
	}


	BENCHMARK("dot scalar") {
		return sum_dot_scalar(A, B);
	};

	BENCHMARK("dot AVX") {
		return sum_dot_avx(A, B);
	};

}
