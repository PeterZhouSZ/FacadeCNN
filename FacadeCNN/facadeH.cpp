﻿#include "facadeH.h"
#include "Utils.h"

cv::Mat generateFacadeH(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, const std::vector<float>& params) {
	// #floors has to be at least 3 for this facade.
	if (range_NF.first < 3) range_NF.first = 3;

	// #columns has to be at least 3 for this facade.
	if (range_NC.first < 3) range_NC.first = 3;

	int NF = std::round(params[0] * (range_NF.second - range_NF.first) + range_NF.first);
	if (NF < range_NF.first) NF = range_NF.first;
	int NC = std::round(params[1] * (range_NC.second - range_NC.first) + range_NC.first);
	if (NC < range_NC.first) NC = range_NC.first;

	float BS = (float)width / (params[7] * 2 + params[8] * (NC - 2)) * params[7];
	float TW = (float)width / (params[7] * 2 + params[8] * (NC - 2)) * params[8];
	float AH = (float)height / (params[9] + params[10] * NF + params[11]) * params[9];
	float FH = (float)height / (params[9] + params[10] * NF + params[11]) * params[10];
	float GH = (float)height / (params[9] + params[10] * NF + params[11]) * params[11];

	float WW = TW / (params[2] + params[4] * 2) * params[2];
	float WH = FH / (params[3] + params[5] + params[6]) * params[3];
	float WS = TW / (params[2] + params[4] * 2) * params[4];
	float WT = FH / (params[3] + params[5] + params[6]) * params[5];
	float WB = FH / (params[3] + params[5] + params[6]) * params[6];

	float WW2 = BS / (params[12] + params[14] + params[15]) * params[12];
	float WO2 = BS / (params[12] + params[14] + params[15]) * params[14];
	float WI2 = BS / (params[12] + params[14] + params[15]) * params[15];
	float WH2 = FH / (params[13] + params[16] + params[17]) * params[13];
	float WT2 = FH / (params[13] + params[16] + params[17]) * params[16];
	float WB2 = FH / (params[13] + params[16] + params[17]) * params[17];

	float DT = GH / (params[18] + params[19]) * params[18];
	float DH = GH / (params[18] + params[19]) * params[19];
	float TWH = AH / (params[20] + params[21] + params[22]) * params[20];
	float TWT = AH / (params[20] + params[21] + params[22]) * params[21];
	float TWB = AH / (params[20] + params[21] + params[22]) * params[22];
	float TWH2 = AH / (params[23] + params[24] + params[25]) * params[23];
	float TWT2 = AH / (params[23] + params[24] + params[25]) * params[24];
	float TWB2 = AH / (params[23] + params[24] + params[25]) * params[25];

	return generateFacadeH(NF, NC, width, height, thickness, WW, WH, WS, WT, WB, BS, TW, AH, FH, GH, WW2, WH2, WO2, WI2, WT2, WB2, DT, DH, TWH, TWT, TWB, TWH2, TWT2, TWB2);
}

cv::Mat generateRandomFacadeH(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement, float window_prob) {
	// #floors has to be at least 1 for this facade.
	if (range_NF.first < 3) range_NF.first = 3;

	// #columns has to be at least 3 for this facade.
	if (range_NC.first < 3) range_NC.first = 3;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::uniform_rand(range_NF.first, range_NF.second + 1);
	int NC = utils::uniform_rand(range_NC.first, range_NC.second + 1);

	// 各フロアの高さ
	float FH = utils::uniform_rand(2.5, 4);

	// １Fの高さ
	float GH = FH + utils::uniform_rand(0, 2);

	// 最上階の高さ
	float AH = FH + utils::uniform_rand(0.2, 2);

	// 各タイルの幅
	float TW = utils::uniform_rand(2, 4);

	// ビルの横マージン
	float BS;
	if (utils::uniform_rand() < 0.2) {
		BS = utils::uniform_rand(TW * 0.5, TW * 0.7);
	}
	else {
		BS = utils::uniform_rand(TW * 1.3, TW * 3);
	}

	// 各フロアの窓上部から天井までの高さ
	float WT = utils::uniform_rand(0.1, 0.35);

	// 各フロアの窓下部からフロア底面までの高さ
	float WB = utils::uniform_rand(0.1, 0.35);

	// 各フロアの窓の高さ
	float WH = utils::uniform_rand(0.3, 0.8);

	// 各フロアの各種高さをnormalize
	ratio = FH / (WT + WB + WH);
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;

	// 左・右端の窓上部から天井までの高さ
	float WT2 = utils::uniform_rand(0.1, 0.35);

	// 左・右端の窓下部からフロア底面までの高さ
	float WB2 = utils::uniform_rand(0.1, 0.35);

	// 左・右端の窓の高さ
	float WH2 = utils::uniform_rand(0.3, 0.8);

	// 左・右端の各種高さをnormalize
	ratio = FH / (WT2 + WB2 + WH2);
	WT2 *= ratio;
	WB2 *= ratio;
	WH2 *= ratio;

	// 各フロアの窓の横マージン
	float WS = utils::uniform_rand(0.05, 0.4);

	// 各フロアの窓の幅
	float WW = utils::uniform_rand(0.2, 0.9);

	// 各フロアの各種幅をnormalize
	ratio = TW / (WS * 2 + WW);
	WS *= ratio;
	WW *= ratio;

	// 左・右端の窓の横のマージン
	float WO2 = utils::uniform_rand(0.2, 0.6);
	float WI2 = utils::uniform_rand(0.2, 0.6);

	// 左・右端の窓の幅
	float WW2 = utils::uniform_rand(0.1, 0.3);

	// 左・右端の各種幅をnormalize
	ratio = BS / (WO2 + WW2 + WI2);
	WO2 *= ratio;
	WW2 *= ratio;
	WI2 *= ratio;

	// 最上階の窓上部から天井までの高さ
	float TWT = utils::uniform_rand(0.1, 1);

	// 最上階の窓下部からフロア底面までの高さ
	float TWB = utils::uniform_rand(0.1, 0.35);

	// 最上階の窓の高さ
	float TWH = utils::uniform_rand(0.3, 0.8);

	// 最上階の各種高さをnormalize
	ratio = AH / (TWT + TWB + TWH);
	TWT *= ratio;
	TWB *= ratio;
	TWH *= ratio;

	// 左右の最上階のドア上部から天井までの高さ
	float TWT2 = utils::uniform_rand(0.2, 2);

	// 左右の最上階の窓下部からフロア底面までの高さ
	float TWB2 = utils::uniform_rand(0.2, 1);

	// 左右の最上階のドアの高さ
	float TWH2 = utils::uniform_rand(1, 2.5);

	// 左右の最上階の各種高さをnormalize
	ratio = AH / (TWT2 + TWB2 + TWH2);
	TWT2 *= ratio;
	TWB2 *= ratio;
	TWH2 *= ratio;

	// １Fのドア上部から天井までの高さ
	float DT = utils::uniform_rand(0.2, 1.5);

	// １Fのドアの高さ
	float DH = utils::uniform_rand(2, 3);

	// １Fの各種高さをnormalize
	ratio = GH / (DT + DH);
	DT *= ratio;
	DH *= ratio;

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (GH + AH + FH * (NF - 2));
	GH *= ratio;
	FH *= ratio;
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	WT2 *= ratio;
	WH2 *= ratio;
	WB2 *= ratio;
	AH *= ratio;
	DT *= ratio;
	DH *= ratio;
	TWT *= ratio;
	TWH *= ratio;
	TWB *= ratio;
	TWT2 *= ratio;
	TWH2 *= ratio;
	TWB2 *= ratio;
	ratio = (float)width / (BS * 2 + TW * (NC - 2));
	BS *= ratio;
	WS *= ratio;
	WW *= ratio;
	TW *= ratio;
	WO2 *= ratio;
	WW2 *= ratio;
	WI2 *= ratio;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータ値を格納
	params.push_back((float)(NF - range_NF.first) / (float)(range_NF.second - range_NF.first));
	params.push_back((float)(NC - range_NC.first) / (float)(range_NC.second - range_NC.first));
	params.push_back(WW / TW);
	params.push_back(WH / FH);
	params.push_back(WS / TW);
	params.push_back(WT / FH);
	params.push_back(WB / FH);
	params.push_back(BS / width);
	params.push_back(TW / width);
	params.push_back(AH / height);
	params.push_back(FH / height);
	params.push_back(GH / height);
	params.push_back(WW2 / BS);
	params.push_back(WH2 / FH);
	params.push_back(WO2 / BS);
	params.push_back(WI2 / BS);
	params.push_back(WT2 / FH);
	params.push_back(WB2 / FH);
	params.push_back(DT / GH);
	params.push_back(DH / GH);
	params.push_back(TWH / AH);
	params.push_back(TWT / AH);
	params.push_back(TWB / AH);
	params.push_back(TWH2 / AH);
	params.push_back(TWT2 / AH);
	params.push_back(TWB2 / AH);

	return generateFacadeH(NF, NC, width, height, thickness, WW, WH, WS, WT, WB, BS, TW, AH, FH, GH, WW2, WH2, WO2, WI2, WT2, WB2, DT, DH, TWH, TWT, TWB, TWH2, TWT2, TWB2, window_displacement, window_prob);
}

cv::Mat generateFacadeH(int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float BS, float TW, float AH, float FH, float GH, float WW2, float WH2, float WO2, float WI2, float WT2, float WB2, float DT, float DH, float TWH, float TWT, float TWB, float TWH2, float TWT2, float TWB2, int window_displacement, float window_prob) {
	cv::Mat result(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

	// １Fのドアを描画
	{
		// 左端
		{
			int x1 = WO2;
			int y1 = height - DH;
			int x2 = WO2 + WW2;
			int y2 = height - 1;

			if (window_displacement > 0) {
				x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
			}

			if (utils::uniform_rand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}

		for (int j = 0; j < NC - 2; ++j) {
			int x1 = BS + TW * j + WS;
			int y1 = height - DH;
			int x2 = BS + TW * j + WS + WW;
			int y2 = height - 1;

			if (window_displacement > 0) {
				x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
			}

			if (utils::uniform_rand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}

		// 右端
		{
			int x1 = BS + TW * (NC - 2) + WI2;
			int y1 = height - DH;
			int x2 = BS + TW * (NC - 2) + WI2 + WW2;
			int y2 = height - 1;

			if (window_displacement > 0) {
				x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
			}

			if (utils::uniform_rand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}
	}

	// ２F以上の窓を描画
	for (int i = 0; i < NF - 2; ++i) {
		// 左端
			{
				int x1 = WO2;
				int y1 = height - GH - FH * i - WB2 - WH2;
				int x2 = WO2 + WW2;
				int y2 = height - GH - FH * i - WB2;

				if (window_displacement > 0) {
					x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
					y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
					x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
					y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				}

				if (utils::uniform_rand() < window_prob) {
					cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
				}
			}

		for (int j = 0; j < NC - 2; ++j) {
			int x1 = BS + TW * j + WS;
			int y1 = height - GH - FH * i - WB - WH;
			int x2 = BS + TW * j + WS + WW;
			int y2 = height - GH - FH * i - WB;

			if (window_displacement > 0) {
				x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
			}

			if (utils::uniform_rand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}

		// 右端
		{
			int x1 = BS + TW * (NC - 2) + WI2;
			int y1 = height - GH - FH * i - WB2 - WH2;
			int x2 = BS + TW * (NC - 2) + WI2 + WW2;
			int y2 = height - GH - FH * i - WB2;

			if (window_displacement > 0) {
				x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
			}

			if (utils::uniform_rand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}
	}

	// 最上階の窓を描画
	{
		// 左端
		{
			int x1 = WO2;
			int y1 = height - GH - FH * (NF - 2) - TWB2 - TWH2;
			int x2 = WO2 + WW2;
			int y2 = height - GH - FH * (NF - 2) - TWB2;

			if (window_displacement > 0) {
				x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
			}

			if (utils::uniform_rand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}

		for (int j = 0; j < NC - 2; ++j) {
			int x1 = BS + TW * j + WS;
			int y1 = height - GH - FH * (NF - 2) - TWB - TWH;
			int x2 = BS + TW * j + WS + WW;
			int y2 = height - GH - FH * (NF - 2) - TWB;

			if (window_displacement > 0) {
				x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
			}

			if (utils::uniform_rand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}

		// 右端
		{
			int x1 = BS + TW * (NC - 2) + WI2;
			int y1 = height - GH - FH * (NF - 2) - TWB2 - TWH2;
			int x2 = BS + TW * (NC - 2) + WI2 + WW2;
			int y2 = height - GH - FH * (NF - 2) - TWB2;

			if (window_displacement > 0) {
				x1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y1 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				x2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
				y2 += utils::uniform_rand(-window_displacement, window_displacement + 1);
			}

			if (utils::uniform_rand() < window_prob) {
				cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), thickness);
			}
		}
	}

	return result;
}