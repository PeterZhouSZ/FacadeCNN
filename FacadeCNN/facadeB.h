#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat generateFacadeB(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, const std::vector<float>& params);
cv::Mat generateRandomFacadeB(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement = 0, float window_prob = 1);
cv::Mat generateFacadeB(int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float BS, float TW, float AH, float FH, float GH, int ND, float DT, float DH, float DW, float DS, int window_displacement = 0, float window_prob = 1);
