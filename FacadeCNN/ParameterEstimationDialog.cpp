#include "ParameterEstimationDialog.h"
#include <QFileDialog>

ParameterEstimationDialog::ParameterEstimationDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	//ui.lineEditTestDataDirectory->setText("C:/Anaconda/caffe/facade/data");
	ui.lineEditTestDataDirectory->setText("//cuda.cs.purdue.edu/scratch2/facade/data");
	ui.lineEditOutputDirectory->setText("results");
	ui.lineEditNumFloorsMin->setText("1");
	ui.lineEditNumFloorsMax->setText("20");
	ui.lineEditNumColumnsMin->setText("1");
	ui.lineEditNumColumnsMax->setText("20");
	ui.checkBoxUseTrueGrammarId->setChecked(true);

	connect(ui.pushButtonTestDataDirectory, SIGNAL(clicked()), this, SLOT(onTestDataDirectory()));
	connect(ui.pushButtonOutputDirectory, SIGNAL(clicked()), this, SLOT(onOutputDirectory()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

ParameterEstimationDialog::~ParameterEstimationDialog() {
}

void ParameterEstimationDialog::onTestDataDirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditTestDataDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditTestDataDirectory->setText(dir);
	}
}

void ParameterEstimationDialog::onOutputDirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditOutputDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditOutputDirectory->setText(dir);
	}
}

void ParameterEstimationDialog::onOK() {
	accept();
}

void ParameterEstimationDialog::onCancel() {
	reject();
}
