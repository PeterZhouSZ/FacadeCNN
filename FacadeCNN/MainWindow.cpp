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
#include "facadeE.h"
#include "facadeF.h"
#include "facadeG.h"
#include "facadeH.h"
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <QTextStream>
#include "FacadeSegmentation.h"

#ifndef SQR
#define SQR(x)	((x) * (x))
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionGenerateTrainingImages, SIGNAL(triggered()), this, SLOT(onGenerateTrainingImages()));
	connect(ui.actionParameterEstimationAll, SIGNAL(triggered()), this, SLOT(onParameterEstimationAll()));
	connect(ui.actionParameterEstimation, SIGNAL(triggered()), this, SLOT(onParameterEstimation()));
}

void MainWindow::onGenerateTrainingImages() {
	generateTrainingImages();
}

/**
* This is called when the user clickes [Tool] -> [Predict]
*/
void MainWindow::onParameterEstimationAll() {
	parameterEstimationAll();
}

void MainWindow::generateTrainingImages() {
	ImageGenerationDialog dlg;
	if (!dlg.exec()) return;

	int NUM_GRAMMARS = 8;

	QString DATA_ROOT = dlg.ui.lineEditOutputDirectory->text();
	int NUM_IMAGES_PER_SNIPPET = dlg.ui.lineEditNumImages->text().toInt();
	int IMAGE_SIZE = dlg.ui.lineEditImageSize->text().toInt();
	bool GRAYSCALE = dlg.ui.checkBoxGrayscale->isChecked();
	float EDGE_DISPLACEMENT = dlg.ui.lineEditEdgeDisplacement->text().toFloat() * 0.01;
	float MISSING_WINDOWS = dlg.ui.lineEditMissingWindows->text().toFloat() * 0.01;

	QString dir = QString(DATA_ROOT);
	if (QDir(dir).exists()) {
		QDir(dir).removeRecursively();
	}
	QDir().mkdir(dir);

	for (int facade_grammar_id = 0; facade_grammar_id < NUM_GRAMMARS; ++facade_grammar_id) {
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
			cv::Mat img = generateFacadeStructure(facade_grammar_id, IMAGE_SIZE, IMAGE_SIZE, params, EDGE_DISPLACEMENT, 1 - MISSING_WINDOWS);

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

cv::Mat MainWindow::generateFacadeStructure(int facade_grammar_id, int width, int height, std::vector<float>& params, float window_displacement, float window_prob) {
	int thickness = 1;
	//int thickness = utils::uniform_rand(1, 4);

	cv::Mat result;
	if (facade_grammar_id == 0) {
		result = FacadeA::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 1) {
		result = FacadeB::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 2) {
		result = FacadeC::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 3) {
		result = FacadeD::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 4) {
		result = FacadeE::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 5) {
		result = FacadeF::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 6) {
		result = FacadeG::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 7) {
		result = FacadeH::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}

	return result;
}

void MainWindow::parameterEstimationAll() {
	ParameterEstimationDialog dlg;
	if (!dlg.exec()) return;

	int NUM_GRAMMARS = 8;

	QString results_dir = dlg.ui.lineEditOutputDirectory->text() + "/";
	if (QDir(results_dir).exists()) {
		QDir(results_dir).removeRecursively();
	}
	if (!QDir().mkdir(results_dir)) {
		std::cerr << "Output directory, " << dlg.ui.lineEditOutputDirectory->text().toUtf8().constData() << " cannot be created." << std::endl;
		return;
	}

	Classifier classifier("models/facade/deploy.prototxt", "models/facade/train_iter_40000.caffemodel", "models/facade/mean.binaryproto");
	std::cout << "Recognition CNN was successfully loaded." << std::endl;

	std::vector<boost::shared_ptr<Regression>> regressions(NUM_GRAMMARS);
	for (int i = 0; i < NUM_GRAMMARS; ++i) {
		QString deploy_name = QString("models/facade/deploy_%1.prototxt").arg(i + 1, 2, 10, QChar('0'));
		QString model_name = QString("models/facade/train_%1_iter_40000.caffemodel").arg(i + 1, 2, 10, QChar('0'));
		regressions[i] = boost::shared_ptr<Regression>(new Regression(deploy_name.toUtf8().constData(), model_name.toUtf8().constData()));
	}
	std::cout << "Parameter estimation CNNs were successfully loaded." << std::endl;

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
	printf("Predicting: ");
	while (true) {
		printf("\rPredicting: %d", iter + 1);

		QString line = in.readLine();
		QStringList list = line.split(" ");
		if (list.size() < 2) break;

		QString file_path = list[0];
		int grammar_id = list[1].toInt();
		if (grammar_id >= NUM_GRAMMARS) continue;

		// obtain file id
		int index1 = file_path.lastIndexOf("/");
		int index2 = file_path.indexOf(".", index1);
		int file_id = file_path.mid(index1 + 1, index2 - index1 - 1).toInt();

		// read the test image
		cv::Mat img = cv::imread((dlg.ui.lineEditTestDataDirectory->text() + "/images/" + file_path).toUtf8().constData());

		// resize the image to 227x227
		cv::resize(img, img, cv::Size(227, 227));

		// classification
		std::vector<Prediction> predictions = classifier.Classify(img, NUM_GRAMMARS);
		if (predictions[0].first == grammar_id) correct_classification++;
		else incorrect_classification++;

		if (dlg.ui.checkBoxUseTrueGrammarId->isChecked()) {
			// パラメータ推定のために、正しいgrammar_idを使用する
			predictions[0].first = grammar_id;
		}

		if (predictions[0].first >= NUM_GRAMMARS) continue;

		// parameter estimation
		std::vector<float> predicted_params = regressions[predictions[0].first]->Predict(img);

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
			predicted_img = FacadeA::generateFacade(img.cols, img.rows, 2, 99, 99, predicted_params);
		}
		else if (predictions[0].first == 1) {
			predicted_img = FacadeB::generateFacade(img.cols, img.rows, 2, 99, 99, predicted_params);
		}
		else if (predictions[0].first == 2) {
			predicted_img = FacadeC::generateFacade(img.cols, img.rows, 2, 99, 99, predicted_params);
		}
		else if (predictions[0].first == 3) {
			predicted_img = FacadeD::generateFacade(img.cols, img.rows, 2, 99, 99, predicted_params);
		}
		else if (predictions[0].first == 4) {
			predicted_img = FacadeE::generateFacade(img.cols, img.rows, 2, 99, 99, predicted_params);
		}
		else if (predictions[0].first == 5) {
			predicted_img = FacadeF::generateFacade(img.cols, img.rows, 2, 99, 99, predicted_params);
		}
		else if (predictions[0].first == 6) {
			predicted_img = FacadeG::generateFacade(img.cols, img.rows, 2, 99, 99, predicted_params);
		}
		else if (predictions[0].first == 7) {
			predicted_img = FacadeH::generateFacade(img.cols, img.rows, 2, 99, 99, predicted_params);
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
		sprintf(filename2, (results_dir + "%02d_%06d_input.png").toUtf8().constData(), grammar_id + 1, iter);
		cv::imwrite(filename2, img);

		char filename[256];
		sprintf(filename, (results_dir + "%02d_%06d_pred.png").toUtf8().constData(), grammar_id + 1, iter);
		cv::imwrite(filename, predicted_img);

		iter++;
	}
	printf("\n");

	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Classification accuracy: " << (float)correct_classification / (correct_classification + incorrect_classification) << std::endl;
	std::cout << std::endl;

	// 誤差を計算
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Parameter estimation RMSE:" << std::endl;
	for (int i = 0; i < rmse.size(); ++i) {
		std::cout << "Grammar #" << i + 1 << ":" << std::endl;
		for (int j = 0; j < rmse[i].size(); ++j) {
			if (j > 0) std::cout << ", ";
			rmse[i][j] =  sqrt(rmse[i][j] / rmse_count[i]);
			std::cout << std::setprecision(4) << rmse[i][j];
		}
		std::cout << std::endl;
	}
}

void MainWindow::onParameterEstimation() {
	int NUM_GRAMMARS = 8;

	QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
	if (filename.isEmpty()) return;
	cv::Mat facade_img = cv::imread(filename.toUtf8().constData());




	Regression floors_regression("models/floors/deploy_01.prototxt", "models/floors/train_01_iter_40000.caffemodel");
	Classifier win_exist_classifier("models/window_existence/deploy.prototxt", "models/window_existence/train_iter_40000.caffemodel", "models/window_existence/mean.binaryproto");
	Regression win_pos_regression("models/window_position/deploy_01.prototxt", "models/window_position/train_01_iter_60000.caffemodel");

	// floor height / column width
	cv::Mat resized_facade_img;
	cv::resize(facade_img, resized_facade_img, cv::Size(227, 227));
	std::vector<float> floor_params = floors_regression.Predict(resized_facade_img);
	int num_floors = std::round(floor_params[0] + 0.3);
	int num_columns = std::round(floor_params[1] + 0.38);
	float average_floor_height = (float)facade_img.rows / num_floors;
	float average_column_width = (float)facade_img.cols / num_columns;

	//////////////////////////////////////////////////////////////////////////////////
	// DEBUG
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "#floors = " << num_floors << ", #columns = " << num_columns << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	//////////////////////////////////////////////////////////////////////////////////

	// subdivide the facade into tiles and windows
	std::vector<float> x_splits;
	std::vector<float> y_splits;
	std::vector<std::vector<fs::WindowPos>> win_rects;
	fs::subdivideFacade(facade_img, average_floor_height, average_column_width, y_splits, x_splits, win_rects);
	std::cout << "#floors: " << num_floors << ", #columns: " << num_columns << std::endl;
	utils::output_vector(x_splits);

#if 1
	// gray scale
	cv::Mat gray_img;
	cv::cvtColor(facade_img, gray_img, cv::COLOR_BGR2GRAY);
	cv::cvtColor(gray_img, gray_img, cv::COLOR_GRAY2BGR);

	for (int i = 0; i < y_splits.size() - 1; ++i) {
		for (int j = 0; j < x_splits.size() - 1; ++j) {
			cv::Mat tile_img(gray_img, cv::Rect(x_splits[j], y_splits[i], x_splits[j + 1] - x_splits[j] + 1, y_splits[i + 1] - y_splits[i] + 1));

			cv::Mat resized_tile_img;
			cv::resize(tile_img, resized_tile_img, cv::Size(227, 227));

			// check the existence of window
			std::vector<Prediction> pred_exist = win_exist_classifier.Classify(resized_tile_img, 2);
			if (pred_exist[0].first == 1) {
				win_rects[i][j].valid = fs::WindowPos::VALID;
			}
			else {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}

			if (fs::WindowPos::VALID) {
				// predict the window position
				std::vector<float> pred_params = win_pos_regression.Predict(resized_tile_img);
				//utils::output_vector(pred_params);
				win_rects[i][j].left = std::round(pred_params[0] * tile_img.cols);
				win_rects[i][j].top = std::round(pred_params[1] * tile_img.rows);
				win_rects[i][j].right = std::round(pred_params[2] * tile_img.cols);
				win_rects[i][j].bottom = std::round(pred_params[3] * tile_img.rows);
			}
		}
	}
#endif

	// generate input image for facade CNN
	cv::Mat input_img;
	fs::generateWindowImage(y_splits, x_splits, win_rects, cv::Size(227, 227), input_img);

	Classifier fac_classifier("models/facade/deploy.prototxt", "models/facade/train_iter_40000.caffemodel", "models/facade/mean.binaryproto");
	std::vector<boost::shared_ptr<Regression>> fac_regressions(NUM_GRAMMARS);
	fac_regressions[0] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_01.prototxt", "models/facade/train_01_iter_40000.caffemodel"));
	fac_regressions[1] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_02.prototxt", "models/facade/train_02_iter_40000.caffemodel"));
	fac_regressions[2] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_03.prototxt", "models/facade/train_03_iter_40000.caffemodel"));
	fac_regressions[3] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_04.prototxt", "models/facade/train_04_iter_40000.caffemodel"));
	fac_regressions[4] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_05.prototxt", "models/facade/train_05_iter_40000.caffemodel"));
	fac_regressions[5] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_06.prototxt", "models/facade/train_06_iter_40000.caffemodel"));
	fac_regressions[6] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_07.prototxt", "models/facade/train_07_iter_40000.caffemodel"));
	fac_regressions[7] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_08.prototxt", "models/facade/train_08_iter_40000.caffemodel"));

	// classification
	std::vector<Prediction> fac_predictions = fac_classifier.Classify(input_img, NUM_GRAMMARS);
	int facade_id = fac_predictions[0].first;
	for (int i = 0; i < fac_predictions.size(); ++i) {
		std::cout << fac_predictions[i].first << ": " << fac_predictions[i].second << std::endl;
	}
	std::cout << "facade grammar: " << facade_id + 1 << std::endl;
	facade_id = 3;
	// parameter estimation
	std::vector<float> predicted_params = fac_regressions[facade_id]->Predict(input_img);
	utils::output_vector(predicted_params);



	// predictされた画像を作成する
	cv::Mat predicted_img;
	if (facade_id == 0) {
		predicted_img = FacadeA::generateFacade(facade_img.cols, facade_img.rows, 1, y_splits.size() - 1, x_splits.size() - 1, predicted_params);
	}
	else if (facade_id == 1) {
		predicted_img = FacadeB::generateFacade(facade_img.cols, facade_img.rows, 1, y_splits.size() - 1, x_splits.size() - 1, predicted_params);
	}
	else if (facade_id == 2) {
		predicted_img = FacadeC::generateFacade(facade_img.cols, facade_img.rows, 1, y_splits.size() - 1, x_splits.size() - 1, predicted_params);
	}
	else if (facade_id == 3) {
		predicted_img = FacadeD::generateFacade(facade_img.cols, facade_img.rows, 1, y_splits.size() - 1, x_splits.size() - 1, predicted_params);
	}
	else if (facade_id == 4) {
		predicted_img = FacadeE::generateFacade(facade_img.cols, facade_img.rows, 1, y_splits.size() - 1, x_splits.size() - 1, predicted_params);
	}
	else if (facade_id == 5) {
		predicted_img = FacadeF::generateFacade(facade_img.cols, facade_img.rows, 1, y_splits.size() - 1, x_splits.size() - 1, predicted_params);
	}
	else if (facade_id == 6) {
		predicted_img = FacadeG::generateFacade(facade_img.cols, facade_img.rows, 1, y_splits.size() - 1, x_splits.size() - 1, predicted_params);
	}
	else if (facade_id == 7) {
		predicted_img = FacadeH::generateFacade(facade_img.cols, facade_img.rows, 1, y_splits.size() - 1, x_splits.size() - 1, predicted_params);
	}

#if 0
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
#endif

	cv::imwrite("input.png", input_img);
	cv::imwrite("result.png", predicted_img);




#if 0
	/////////////////////////////////////////////////////////////////////////////////
	// window geometry detection
	Classifier win_classifier("models/window/deploy.prototxt", "models/window/train_iter_20000.caffemodel", "models/window/mean.binaryproto");

	// cluster the tiles based on the grammar
	int num_window_types;
	if (facade_id == 0) {
		num_window_types = FacadeA::clusterWindowTypes(win_rects);
	}
	else if (facade_id == 1) {
		num_window_types = FacadeB::clusterWindowTypes(win_rects);
	}
	else if (facade_id == 2) {
		num_window_types = FacadeC::clusterWindowTypes(win_rects);
	}
	else if (facade_id == 3) {
		num_window_types = FacadeD::clusterWindowTypes(win_rects);
	}
	else if (facade_id == 4) {
		num_window_types = FacadeE::clusterWindowTypes(win_rects);
	}
	else if (facade_id == 5) {
		num_window_types = FacadeF::clusterWindowTypes(win_rects);
	}
	else if (facade_id == 6) {
		num_window_types = FacadeG::clusterWindowTypes(win_rects);
	}
	else if (facade_id == 7) {
		num_window_types = FacadeH::clusterWindowTypes(win_rects);
	}

	std::cout << "-------------------------------------" << std::endl;
	std::cout << "facade: " << facade_id + 1 << std::endl;
	std::cout << "-------------------------------------" << std::endl;
	std::cout << "window:" << std::endl;
	std::map<int, std::vector<int>> win_type_votes;
	for (int i = 0; i < y_splits.size() - 1; ++i) {
		for (int j = 0; j < x_splits.size() - 1; ++j) {
			if (j > 0) std::cout << ", ";
			if (win_rects[i][j].valid == fs::WindowPos::VALID) {
				int x = x_splits[j];
				int w = x_splits[j + 1] - x_splits[j];
				int y = y_splits[i];
				int h = y_splits[i + 1] - y_splits[i];

				cv::Mat tile_img(img, cv::Rect(x, y, w, h));

				cv::Mat tile_img227;
				cv::resize(tile_img, tile_img227, cv::Size(227, 227));
				
				//////////////////////////////////////////////////////////////////
				// DEBUG
				char filename[256];
				sprintf(filename, "results/tile_%d_%d.png", i, j);
				cv::imwrite(filename, tile_img227);
				//////////////////////////////////////////////////////////////////


				std::vector<Prediction> win_predictions = win_classifier.Classify(tile_img227, 13);
				int win_id = win_predictions[0].first;
				std::cout << win_id + 1 << "(" << win_rects[i][j].type << ")";

				win_type_votes[win_rects[i][j].type].push_back(win_id);
			}
			else {
				std::cout << " " << "(" << win_rects[i][j].type << ")";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	// find the maximum vote for each window group
	std::map<int, int> selected_win_types;
	for (int i = 0; i < num_window_types; ++i) {
		std::map<int, int> votes;	// mapping from window type to #votes
		int max_votes = 0;
		int selected_win_type = 0;

		for (int k = 0; k < win_type_votes[i].size(); ++k) {
			votes[win_type_votes[i][k]]++;
			if (votes[win_type_votes[i][k]] > max_votes) {
				max_votes = votes[win_type_votes[i][k]];
				selected_win_type = win_type_votes[i][k];
			}
		}

		selected_win_types[i] = selected_win_type;
		std::cout << "window group=" << i << ": type=" << selected_win_type + 1 << std::endl;
	}
#endif
}

