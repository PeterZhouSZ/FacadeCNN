#include "facadeD.h"
#include "Utils.h"

cv::Mat generateFacadeD(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, const std::vector<float>& params) {
	// #floors has to be at least 3 for this facade.
	if (range_NF.first < 3) range_NF.first = 3;

	int NF = params[0] * (range_NF.second - range_NF.first) + 0.5 + range_NF.first;
	int NC = params[1] * (range_NC.second - range_NC.first) + 0.5 + range_NC.first;

	float BS = (float)width / (params[12] * 2 + params[13] * NC) * params[12];
	float TW = (float)width / (params[12] * 2 + params[13] * NC) * params[13];
	float AH = (float)height / (params[14] + params[15] * (NF - 2) + params[16] + params[17]) * params[14];
	float FH = (float)height / (params[14] + params[15] * (NF - 2) + params[16] + params[17]) * params[15];
	float FH2 = (float)height / (params[14] + params[15] * (NF - 2) + params[16] + params[17]) * params[16];
	float GH = (float)height / (params[14] + params[15] * (NF - 2) + params[16] + params[17]) * params[17];

	float WW = TW / (params[2] + params[4] * 2) * params[2];
	float WH = FH / (params[3] + params[5] + params[6]) * params[3];
	float WS = TW / (params[2] + params[4] * 2) * params[4];
	float WT = FH / (params[3] + params[5] + params[6]) * params[5];
	float WB = FH / (params[3] + params[5] + params[6]) * params[6];

	float WW2 = TW / (params[7] + params[9] * 2) * params[7];
	float WH2 = FH2 / (params[8] + params[10] + params[11]) * params[8];
	float WS2 = TW / (params[7] + params[9] * 2) * params[9];
	float WT2 = FH2 / (params[8] + params[10] + params[11]) * params[10];
	float WB2 = FH2 / (params[8] + params[10] + params[11]) * params[11];

	// １Fのドアの数 {1, NC/4, NC/3, NC/2, NC}
	int ND_type = std::round(params[18] * 4);
	if (ND_type < 0) ND_type = 0;
	if (ND_type > 4) ND_type = 4;

	int ND;
	if (ND_type == 0) {
		ND = 1;
	}
	else if (ND_type == 1) {
		ND = std::ceil((float)NC / 4);
	}
	else if (ND_type == 2) {
		ND = std::ceil((float)NC / 3);
	}
	else if (ND_type == 3) {
		ND = std::ceil((float)NC / 2);
	}
	else if (ND_type == 4) {
		ND = NC;
	}

	float DT = GH / (params[19] + params[20]) * params[19];
	float DH = GH / (params[19] + params[20]) * params[20];
	float DW = TW * NC / ND / (params[21] + params[22] * 2) * params[21];
	float DS = TW * NC / ND / (params[21] + params[22] * 2) * params[22];

	return generateFacadeD(NF, NC, width, height, thickness, WW, WH, WS, WT, WB, WW2, WH2, WS2, WT2, WB2, BS, TW, AH, FH, FH2, GH, ND, DT, DH, DW, DS);
}

cv::Mat generateRandomFacadeD(int width, int height, int thickness, std::pair<int, int> range_NF, std::pair<int, int> range_NC, std::vector<float>& params, int window_displacement, float window_prob) {
	// #floors has to be at least 3 for this facade.
	if (range_NF.first < 3) range_NF.first = 3;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータを設定
	float ratio;

	int NF = utils::uniform_rand(range_NF.first, range_NF.second + 1);
	int NC = utils::uniform_rand(range_NC.first, range_NC.second + 1);

	// 屋根の高さ
	float AH = utils::uniform_rand(0, 0.5);

	// 各フロアの高さ
	float FH = utils::uniform_rand(2.5, 4);

	// １Fの高さ
	float GH = FH + utils::uniform_rand(0, 2);

	// ２Fの高さ
	float FH2 = FH + utils::uniform_rand(-1, 2);

	// 各タイルの幅
	float TW = utils::uniform_rand(2, 4);

	// ビルの横マージン
	float BS = utils::uniform_rand(0, 1);

	// 各フロアの窓上部から天井までの高さ
	float WT = utils::uniform_rand(0.2, 1);

	// 各フロアの窓下部からフロア底面までの高さ
	float WB = utils::uniform_rand(0.2, 1);

	// 各フロアの窓の高さ
	float WH = utils::uniform_rand(1, 2.5);

	// 各フロアの各種高さをnormalize
	ratio = FH / (WT + WB + WH);
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;

	// 各フロアの窓の横マージン
	float WS = utils::uniform_rand(0.2, 1);

	// 各フロアの窓の幅
	float WW = utils::uniform_rand(0.5, 2.5);

	// 各フロアの各種幅をnormalize
	ratio = TW / (WS * 2 + WW);
	WS *= ratio;
	WW *= ratio;

	// 2Fの窓上部から天井までの高さ
	float WT2 = utils::uniform_rand(0.2, 1);

	// 2Fの窓下部からフロア底面までの高さ
	float WB2 = utils::uniform_rand(0.2, 1);

	// 2Fの窓の高さ
	float WH2 = utils::uniform_rand(1, 2.5);

	// 2Fの各種高さをnormalize
	ratio = FH2 / (WT2 + WB2 + WH2);
	WT2 *= ratio;
	WB2 *= ratio;
	WH2 *= ratio;

	// 2Fの窓の横マージン
	float WS2 = utils::uniform_rand(0.2, 1);

	// 2Fの窓の幅
	float WW2 = utils::uniform_rand(0.5, 2.5);

	// 2Fの各種幅をnormalize
	ratio = TW / (WS2 * 2 + WW2);
	WS2 *= ratio;
	WW2 *= ratio;
	
	// １Fのドア上部から天井までの高さ
	float DT = utils::uniform_rand(0.2, 1.5);

	// １Fのドアの高さ
	float DH = utils::uniform_rand(2, 3);

	// １Fの各種高さをnormalize
	ratio = GH / (DT + DH);
	DT *= ratio;
	DH *= ratio;

	// １Fのドアの数 {1, NC/4, NC/3, NC/2, N}
	int ND;
	int ND_type = utils::uniform_rand(0, 5);
	if (ND_type == 0) {
		ND = 1;
	}
	else if (ND_type == 1) {
		ND = std::ceil((float)NC / 4);
	}
	else if (ND_type == 2) {
		ND = std::ceil((float)NC / 3);
	}
	else if (ND_type == 3) {
		ND = std::ceil((float)NC / 2);
	}
	else if (ND_type == 4) {
		ND = NC;
	}

	// １Fのドアの横マージン
	float DS = utils::uniform_rand(0.2, 1);

	// １Fのドアの幅
	float DW = utils::uniform_rand(0.5, 4);

	// １Fの各種幅をnormalize
	ratio = TW * NC / ND / (DS * 2 + DW);
	DS *= ratio;
	DW *= ratio;

	// すべてを画像のサイズにnormalize
	ratio = (float)height / (GH + AH + FH * (NF - 1));
	GH *= ratio;
	FH *= ratio;
	FH2 *= ratio;
	WT *= ratio;
	WB *= ratio;
	WH *= ratio;
	WT2 *= ratio;
	WB2 *= ratio;
	WH2 *= ratio;
	AH *= ratio;
	DH *= ratio;
	DT *= ratio;
	ratio = (float)width / (BS * 2 + TW * NC);
	BS *= ratio;
	WS *= ratio;
	WW *= ratio;
	TW *= ratio;
	WS2 *= ratio;
	WW2 *= ratio;
	DS *= ratio;
	DW *= ratio;

	///////////////////////////////////////////////////////////////////////////////////
	// パラメータ値を格納
	params.push_back((float)(NF - range_NF.first) / (float)(range_NF.second - range_NF.first));
	params.push_back((float)(NC - range_NC.first) / (float)(range_NC.second - range_NC.first));
	params.push_back(WW / TW);
	params.push_back(WH / FH);
	params.push_back(WS / TW);
	params.push_back(WT / FH);
	params.push_back(WB / FH);
	params.push_back(WW2 / TW);
	params.push_back(WH2 / FH2);
	params.push_back(WS2 / TW);
	params.push_back(WT2 / FH2);
	params.push_back(WB2 / FH2);
	params.push_back(BS / width);
	params.push_back(TW / width);
	params.push_back(AH / height);
	params.push_back(FH / height);
	params.push_back(FH2 / height);
	params.push_back(GH / height);
	params.push_back((float)ND_type / 4);
	params.push_back(DT / GH);
	params.push_back(DH / GH);
	params.push_back(DW / (DW + DS * 2));
	params.push_back(DS / (DW + DS * 2));

	return generateFacadeD(NF, NC, width, height, thickness, WW, WH, WS, WT, WB, WW2, WH2, WS2, WT2, WB2, BS, TW, AH, FH, FH2, GH, ND, DT, DH, DW, DS, window_displacement, window_prob);
}

cv::Mat generateFacadeD(int NF, int NC, int width, int height, int thickness, float WW, float WH, float WS, float WT, float WB, float WW2, float WH2, float WS2, float WT2, float WB2, float BS, float TW, float AH, float FH, float FH2, float GH, int ND, float DT, float DH, float DW, float DS, int window_displacement, float window_prob) {
	cv::Mat result(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

	// １Fのドアを描画
	for (int j = 0; j < ND; ++j) {
		int x1 = BS + (DS * 2 + DW) * j + DS;
		int y1 = height - DH;
		int x2 = BS + (DS * 2 + DW) * j + DS + DW;
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

	// 2Fの窓を描画
	for (int j = 0; j < NC; ++j) {
		int x1 = BS + TW * j + WS2;
		int y1 = height - GH - WB2 - WH2;
		int x2 = BS + TW * j + WS2 + WW2;
		int y2 = height - GH - WB2;

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

	// 3F以上の窓を描画
	for (int i = 0; i < NF - 1; ++i) {
		for (int j = 0; j < NC; ++j) {
			int x1 = BS + TW * j + WS;
			int y1 = height - GH - FH2 - FH * i - WB - WH;
			int x2 = BS + (WS * 2 + WW) * j + WS + WW;
			int y2 = height - GH - FH2 - FH * i - WB;

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
