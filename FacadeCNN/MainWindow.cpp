#include "MainWindow.h"
#include <QFileDialog>
#include <boost/filesystem.hpp>
#include "Classifier.h"
#include "Regression.h"
#include "Utils.h"
#include "ImageGenerationDialog.h"
#include "ParameterEstimationDialog.h"
#include "facadeA.h"
#include "facadeB.h"
#include "facadeC.h"
#include "facadeD.h"
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <QTextStream>

#ifndef SQR
#define SQR(x)	((x) * (x))
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionGenerateTrainingImages, SIGNAL(triggered()), this, SLOT(onGenerateTrainingImages()));
	connect(ui.actionParameterEstimation, SIGNAL(triggered()), this, SLOT(onParameterEstimation()));
}

void MainWindow::onGenerateTrainingImages() {
	generateTrainingImages();
}

/**
* This is called when the user clickes [Tool] -> [Predict]
*/
void MainWindow::onParameterEstimation() {
	parameterEstimation();
}

void MainWindow::generateTrainingImages() {
	ImageGenerationDialog dlg;
	if (!dlg.exec()) return;

	QString DATA_ROOT = dlg.ui.lineEditOutputDirectory->text();
	int NUM_IMAGES_PER_SNIPPET = dlg.ui.lineEditNumImages->text().toInt();
	int IMAGE_SIZE = dlg.ui.lineEditImageSize->text().toInt();
	bool GRAYSCALE = dlg.ui.checkBoxGrayscale->isChecked();
	float EDGE_DISPLACEMENT = dlg.ui.lineEditEdgeDisplacement->text().toFloat();
	float MISSING_WINDOWS = dlg.ui.lineEditMissingWindows->text().toFloat() * 0.01;
	std::pair<int, int> range_NF = std::make_pair(dlg.ui.lineEditNumFloorsMin->text().toInt(), dlg.ui.lineEditNumFloorsMax->text().toInt());
	std::pair<int, int> range_NC = std::make_pair(dlg.ui.lineEditNumColumnsMin->text().toInt(), dlg.ui.lineEditNumColumnsMax->text().toInt());

	QString dir = QString(DATA_ROOT);
	if (QDir(dir).exists()) {
		QDir(dir).removeRecursively();
	}
	QDir().mkdir(dir);

	for (int facade_grammar_id = 0; facade_grammar_id < 4; ++facade_grammar_id) {
		srand(0);

		QString dirname = QString(DATA_ROOT + "/%1/").arg(facade_grammar_id + 1, 2, 10, QChar('0'));
		QDir().mkdir(dirname);

		int subdir_id = -1;

		QFile file_param(dirname + "parameters.txt");
		file_param.open(QIODevice::WriteOnly);
		QTextStream out(&file_param);

		printf("Grammar snippet #%d:", facade_grammar_id + 1);
		for (int iter = 0; iter < NUM_IMAGES_PER_SNIPPET; ++iter) {
			printf("\rGrammar snippet #%d: %d", facade_grammar_id + 1, iter + 1);

			if (iter % 100000 == 0) {
				subdir_id++;

				QString subdirname = QString(dirname + "%1/").arg(subdir_id, 6, 10, QChar('0'));
				QDir().mkdir(subdirname);
			}

			std::vector<float> params;
			cv::Mat img = generateFacadeStructure(facade_grammar_id, IMAGE_SIZE, IMAGE_SIZE, range_NF, range_NC, params, EDGE_DISPLACEMENT, 1 - MISSING_WINDOWS);

			if (GRAYSCALE) {
				cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
			}

			///////////////////////////////////////////////////
			// save the image to the file
			QString filename = QString(dirname + "%1/%2.png").arg(subdir_id, 6, 10, QChar('0')).arg(iter, 6, 10, QChar('0'));
			cv::imwrite(filename.toUtf8().constData(), img);

			for (int i = 0; i < params.size(); ++i) {
				if (i > 0) out << ",";
				out << params[i];
			}
			out << "\n";
		}
		printf("\n");

		file_param.close();
	}
	printf("\n");
}

cv::Mat MainWindow::generateFacadeStructure(int facade_gramamr_id, int width, int height, const std::pair<int, int>& range_NF, const std::pair<int, int>& range_NC, std::vector<float>& params, int window_displacement, float window_prob) {
	int thickness = 1;
	//int thickness = utils::uniform_rand(1, 4);

	cv::Mat result;
	if (facade_gramamr_id == 0) {
		result = generateRandomFacadeA(width, height, thickness, range_NF, range_NC, params, window_displacement, window_prob);
	}
	else if (facade_gramamr_id == 1) {
		result = generateRandomFacadeB(width, height, thickness, range_NF, range_NC, params, window_displacement, window_prob);
	}
	else if (facade_gramamr_id == 2) {
		result = generateRandomFacadeC(width, height, thickness, range_NF, range_NC, params, window_displacement, window_prob);
	}
	else if (facade_gramamr_id == 3) {
		result = generateRandomFacadeD(width, height, thickness, range_NF, range_NC, params, window_displacement, window_prob);
	}

	return result;
}

void MainWindow::parameterEstimation() {
	ParameterEstimationDialog dlg;
	if (!dlg.exec()) return;

	int NUM_GRAMMARS = 2;

	QString results_dir = dlg.ui.lineEditOutputDirectory->text() + "/";
	if (QDir(results_dir).exists()) {
		QDir(results_dir).removeRecursively();
	}
	QDir().mkdir(results_dir);

	std::pair<int, int> range_NF = std::make_pair(dlg.ui.lineEditNumFloorsMin->text().toInt(), dlg.ui.lineEditNumFloorsMax->text().toInt());
	std::pair<int, int> range_NC = std::make_pair(dlg.ui.lineEditNumColumnsMin->text().toInt(), dlg.ui.lineEditNumColumnsMax->text().toInt());

	Classifier classifier((dlg.ui.lineEditClassificationDirectory->text() + "/model/deploy.prototxt").toUtf8().constData(), (dlg.ui.lineEditClassificationDirectory->text() + "/model/train_iter_20000.caffemodel").toUtf8().constData(), "");
	std::vector<boost::shared_ptr<Regression>> regressions(4);
	for (int i = 0; i < NUM_GRAMMARS; ++i) {
		QString deploy_name = dlg.ui.lineEditRegressionDirectory->text() + QString("/model/deploy_%1.prototxt").arg(i + 1, 2, 10, QChar('0'));
		QString model_name = dlg.ui.lineEditRegressionDirectory->text() + QString("/model/train_%1_iter_40000.caffemodel").arg(i + 1, 2, 10, QChar('0'));
		regressions[i] = boost::shared_ptr<Regression>(new Regression(deploy_name.toUtf8().constData(), model_name.toUtf8().constData()));
	}

	int correct_classification = 0;
	int incorrect_classification = 0;

	// read the ground truth of parameter values
	std::map<int, std::vector<std::vector<float>>> params_truth;
	for (int i = 0; i < NUM_GRAMMARS; ++i) {
		params_truth[i] = std::vector<std::vector<float>>();

		QString file_path = dlg.ui.lineEditTestDataDirectory->text() + QString("/images/%1/parameters.txt").arg(i + 1, 2, 10, QChar('0'));

		QFile file_params(file_path);
		file_params.open(QIODevice::ReadOnly);
		QTextStream in_params(&file_params);

		while (true) {
			QString line = in_params.readLine();
			if (line.isEmpty()) break;

			QStringList strs = line.split(",");

			std::vector<float> values;
			for (int j = 0; j < strs.size(); ++j) {
				values.push_back(strs[j].toFloat());
			}

			params_truth[i].push_back(values);
		}
	}

	std::map<int, std::vector<float>> rmse;
	std::map<int, int> rmse_count;

	QFile file(dlg.ui.lineEditTestDataDirectory->text() + "/test.txt");
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);
	int iter = 0;
	while (true) {
		QString line = in.readLine();
		QStringList list = line.split(" ");
		if (list.size() < 2) break;

		QString file_path = list[0];
		int grammar_id = list[1].toInt();
		if (grammar_id >= NUM_GRAMMARS) continue;

		// read the test image
		cv::Mat img = cv::imread((dlg.ui.lineEditTestDataDirectory->text() + "/images/" + file_path).toUtf8().constData());

		// resize the image to 227x227
		cv::resize(img, img, cv::Size(227, 227));

		// classification
		std::vector<Prediction> predictions = classifier.Classify(img, 4);
		if (predictions[0].first == grammar_id) correct_classification++;
		else incorrect_classification++;

		// parameter estimation
		std::vector<float> predicted_params = regressions[grammar_id]->Predict(img);

		// obtain file id
		int index1 = file_path.lastIndexOf("/");
		int index2 = file_path.indexOf(".", index1);
		int file_id = file_path.mid(index1 + 1, index2 - index1 - 1).toInt();


		////////////////////////////////////////////////////////////////////////////////////////////////////
		// デバッグ用（classificationが未トレーニングのため、とりあえず

		predictions[0].first = grammar_id;

		////////////////////////////////////////////////////////////////////////////////////////////////////



		// 誤差を計算
		if (predictions[0].first == grammar_id) {
			if (rmse[grammar_id].size() == 0) {
				rmse[grammar_id].resize(predicted_params.size(), 0);
				rmse_count[grammar_id] = 0;
			}

			for (int i = 0; i < predicted_params.size(); ++i) {
				rmse[grammar_id][i] += SQR(params_truth[grammar_id][file_id][i] - predicted_params[i]);
			}
			rmse_count[grammar_id]++;
		}

		// predictされた画像を作成する
		cv::Mat predicted_img;
		if (predictions[0].first == 0) {
			predicted_img = generateFacadeA(img.cols, img.rows, 2, range_NF, range_NC, predicted_params);
		}
		else if (predictions[0].first == 1) {
			predicted_img = generateFacadeB(img.cols, img.rows, 2, range_NF, range_NC, predicted_params);
		}
		else if (predictions[0].first == 2) {
			predicted_img = generateFacadeC(img.cols, img.rows, 2, range_NF, range_NC, predicted_params);
		}
		else if (predictions[0].first == 3) {
			predicted_img = generateFacadeD(img.cols, img.rows, 2, range_NF, range_NC, predicted_params);
		}

		// make the predicted image blue
		for (int r = 0; r < predicted_img.rows; ++r) {
			for (int c = 0; c < predicted_img.cols; ++c) {
				cv::Vec3b color = predicted_img.at<cv::Vec3b>(r, c);
				if (color[0] == 0) {
					predicted_img.at<cv::Vec3b>(r, c) = cv::Vec3b(255, 0, 0);
				}
				else {
					predicted_img.at<cv::Vec3b>(r, c) = cv::Vec3b(255, 255, 255);
				}
			}
		}

		char filename2[256];
		sprintf(filename2, (dlg.ui.lineEditOutputDirectory->text() + "/%02d_%06d_input.png").toUtf8().constData(), grammar_id, iter);
		cv::imwrite(filename2, img);

		char filename[256];
		sprintf(filename, (dlg.ui.lineEditOutputDirectory->text() + "/%02d_%06d_pred.png").toUtf8().constData(), grammar_id, iter);
		cv::imwrite(filename, predicted_img);

		iter++;
	}

	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Classification accuracy: " << (float)correct_classification / (correct_classification + incorrect_classification) << std::endl;
	std::cout << std::endl;

	// 誤差を計算
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Parameter estimation RMSE:" << std::endl;
	for (int i = 0; i < rmse.size(); ++i) {
		for (int j = 0; j < rmse[i].size(); ++j) {
			if (j > 0) std::cout << ", ";
			rmse[i][j] =  sqrt(rmse[i][j] / rmse_count[i]);
			std::cout << rmse[i][j];
		}
		std::cout << std::endl;
	}
}

