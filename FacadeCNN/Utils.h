#pragma once

#include <opencv2/opencv.hpp>

namespace utils {

	double genRand();
	double genRand(double a, double b);
	float gause(float u, float sigma);
	float stddev(std::vector<float> list);
	float mean(std::vector<float> list);

	void output_vector(const std::vector<float>& values);
}