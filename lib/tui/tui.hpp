#pragma once

#include <atomic>
#include <iostream>

namespace tui {

inline constexpr std::string red = "\033[31m";
inline constexpr std::string green = "\033[32m";
inline constexpr std::string yellow = "\033[33m";

inline constexpr std::string reset = "\033[0m";

class LoadingIndicator {
public:

	explicit LoadingIndicator(int work_units) : _work_units(work_units) {}

	void render() {
		const double progress = _work_progress / static_cast<double>(_work_units);
		int filled = static_cast<int>(progress * _width + 0.5);
		std::string bar(filled, '=');
		bar.resize(_width, ' ');
		int percentage = static_cast<int>(progress * 100.0 + 0.5);

		//|r here tells the cursor to go to the beginning of the line, allows us to rewrite existing
		//text from the buffer
		std::cout << "\r[" << bar << "] " << green << percentage << "%";
		std::cout.flush();
	}

	LoadingIndicator& operator++() {
		++_work_progress;
		render();
		return *this;
	}
private:
	std::atomic_int _work_progress = 0;
	int _work_units;

	const int _width = 50;
};


}


