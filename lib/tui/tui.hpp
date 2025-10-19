#pragma once

#include <atomic>
#include <iostream>
#include <print>

namespace tui {

class LoadingIndicator {
public:

	explicit LoadingIndicator(int work_units) : _work_units(work_units) {}

	void render() {
		const double progress = _work_progress / static_cast<double>(_work_units);
		int filled = static_cast<int>(progress * _width + 0.5);
		std::string bar(filled, '=');
		bar.resize(_width, ' ');
		int percentage = static_cast<int>(progress * 100.0 + 0.5);


		std::cout << "\r[" << bar << "] " << percentage << "%";
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


