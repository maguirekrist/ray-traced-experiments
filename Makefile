main: main.cpp vec.cpp ray.cpp hittable.hpp sphere.hpp hittable_list.hpp interval.hpp constants.hpp
	g++ -std=c++23 -Wall -Wextra -o main main.cpp vec.cpp ray.cpp hittable.hpp sphere.hpp hittable_list.hpp interval.hpp constants.hpp
