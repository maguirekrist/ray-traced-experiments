#include "config.hpp"
#include <expected>
#include <fstream>
#include <format>

static std::expected<cfg::KeyValuePair, std::string> parse_line(const std::string& str) {

	int split_index = 0;
	for(size_t i = 0; i < str.length(); i++)
	{
		auto c = str[i];
		if (c == '=')
		{
			split_index = i;
		}
	}

	std::string key = str.substr(0, split_index);
	std::string lit = str.substr(split_index + 1);

	return cfg::KeyValuePair { .key = key, .value_literal = lit };
}

std::expected<cfg::Config, std::string> cfg::parse_file(const std::string &path)
{
	std::ifstream file(path);

	if (file.is_open())
	{
		std::string line;
		Config config;

		while(std::getline(file, line)) {
			//parse line
			auto kv = parse_line(line);
			if (kv.has_value())
			{
				config.kv[kv.value().key] = kv.value();
			} else {
				return std::unexpected(kv.error());
			}
		}
		
		return config;
	}

	return std::unexpected(std::format("Unable to open file: {}", path));
}

std::optional<cfg::KeyValuePair> cfg::Config::get(const std::string& key) const {
	
	if (kv.contains(key)) {
		return kv.at(key);	
	} else {
		return std::nullopt;
	}
};


