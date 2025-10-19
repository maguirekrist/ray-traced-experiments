#include <print>
#define CATCH_CONFIG_MAIN

#include "../third_party/catch2/catch_amalgamated.hpp"
#include "../lib/cfg/config.hpp"

TEST_CASE("parses simple int") {
	auto result = cfg::parse_file("bad.ini");
	REQUIRE(result.has_value() == false);

	auto good_result = cfg::parse_file("tests/test_init.ini");
	REQUIRE(good_result.has_value());

	auto config = good_result.value();

	REQUIRE(config.kv.size() == 2);

	for(auto const& [key, value] : config.kv)
     	{
		std::println("{}", key);
     	}

	REQUIRE(config.get("test_one").has_value());
	REQUIRE(config.get("test_two").has_value());
	REQUIRE(!config.get("test_three").has_value());
	// REQUIRE(config.get("test_one").value().get_value<float>() == 1.0);
	// REQUIRE(config.get("test_two")->get_value<bool>());
}
