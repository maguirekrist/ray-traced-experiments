#pragma once

#include <expected>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <optional>

namespace cfg {
	
	enum class KeyType {
		String,
		Float,
		Int,
	};

	struct KeyValuePair {
		std::string key;
		std::string value_literal;

		template<typename T>
		std::expected<T, std::string> get_value() const {

			if constexpr (std::is_same_v<T, std::string>) {
				return value_literal;
			} else if constexpr (std::is_same_v<T, std::string_view>) {
				return std::string_view(value_literal);
			} else if constexpr (std::is_same_v<T, bool>) {
				return value_literal == "true" ? true : false;
			} else if constexpr (std::is_integral_v<T>) {
				auto v = std::stoi(value_literal);
				return v;
			} else if (std::is_floating_point_v<T>) {
				auto v = std::stof(value_literal);
				return v;
			} else {
				return std::unexpected("Unsupported for type T");
			}
		}
	};

	struct Config {
		std::unordered_map<std::string, KeyValuePair> kv;
		std::optional<KeyValuePair> get(std::string key) const;
	};

	std::expected<Config, std::string> parse_file(const std::string& path);
}
