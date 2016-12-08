#include "Utils.h"
#include <math.h>
#include <random>

namespace utils {
	const float M_PI = 3.1415926535;

	double genRand() {
		return (double)(rand() % 1000) / 1000.0;
	}

	double genRand(double a, double b) {
		return genRand() * (b - a) + a;
	}

	float gause(float u, float sigma) {
		return 1.0f / 2.0f / M_PI / sigma / sigma * expf(-u * u / 2.0f / sigma / sigma);
	}

	float stddev(std::vector<float> list) {
		if (list.size() <= 1) return 0.0f;

		float avg = mean(list);

		float total = 0.0f;
		for (int i = 0; i < list.size(); ++i) {
			total += (list[i] - avg) * (list[i] - avg);
		}

		return sqrt(total / (list.size() - 1));
	}

	float mean(std::vector<float> list) {
		float sum = 0.0f;
		for (int i = 0; i < list.size(); ++i) {
			sum += list[i];
		}
		return sum / list.size();
	}

	void output_vector(const std::vector<float>& values) {
		for (int i = 0; i < values.size(); ++i) {
			if (i > 0) std::cout << ", ";
			std::cout << values[i];
		}
		std::cout << std::endl;
	}

}