/*
 * StereoVisionCalibrationDialog.cpp
 *
 *  Created on: 30/03/2009
 *      Author: David
 */

#include "StereoVisionCalibrationDialog.h"

StereoVisionCalibrationDialog::StereoVisionCalibrationDialog(vector<AbstractCamera*>* camerasRef,
		ThreeDWidget* modelVisualization) :
	_camerasRef(camerasRef), _modelVisualizationRef(modelVisualization), _individuals(30.0f), _generations(1000.0f),
			_differentialVariation(0.84f), _recombinationConstant(0.9f), _maxDepth(1.5f), _rotationMaxInDegrees(
					90.0f), _focalLengthMax(1500.0f), _focalLengthMin(500.0f) {

	QString windowTitle("Stereo Calibration");
	setWindowTitle(windowTitle);

	_mainLayout = new QVBoxLayout();
	createCalibrationPointsBox();
	createAlgorithmParametersBox();
	createCameraParametersBox();
	createCalibrationBox();

	_mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(_mainLayout);

	_recordPointsTimer = new QTimer(this);
	connect(_recordPointsTimer, SIGNAL(timeout()), this, SLOT(recordCalibrationPoint()));

}

void StereoVisionCalibrationDialog::calibrate() {

	Timer t;
	double cost = 0;

	t.start();

	DifferentialEvolutionCalibrator differentialEvolutionCalibrator(_camerasRef, _maxDepth,
			_rotationMaxInDegrees * (CV_PI / 180), _focalLengthMin, _focalLengthMax);

	vector<POI> POIs3d = differentialEvolutionCalibrator.calibrate(_individuals, _generations, _differentialVariation,
			_recombinationConstant, cost, _calibrationPoints, _progresBar);

	QString results = QString("Calibration ended in %1s\nBack projection error = %2\n").arg(t.stop()).arg(cost);
	_resultsText->setText(results);

	for (unsigned int i = 0; i < _camerasRef->size(); i++) {
		_resultsText->append((_camerasRef->at(i))->getInfo());
	}

	_resultsText->append("\nPOIs:\n");
	for (unsigned int i = 0; i < POIs3d.size(); i++) {
		_resultsText->append(POIs3d[i].getInfo3d());
	}

	_modelVisualizationRef->setPointCloud(POIs3d);
	_modelVisualizationRef->update();

}

void StereoVisionCalibrationDialog::createCalibrationPointsBox() {

	QString boxTitle("Calibration Points");
	QGroupBox* calibrationPointsBox = new QGroupBox(boxTitle, this);
	QGridLayout* calibrationPointsLayout = new QGridLayout(calibrationPointsBox);

	_beginRecordingPointsButton = new QPushButton("Begin Recording Points", this);
	connect(_beginRecordingPointsButton, SIGNAL(clicked()), this, SLOT(beginRecordingPoints()));
	calibrationPointsLayout->addWidget(_beginRecordingPointsButton, 0, 0);

	_stopRecordingPointsButton = new QPushButton("Stop Recording Points", this);
	_stopRecordingPointsButton->setDisabled(true);
	connect(_stopRecordingPointsButton, SIGNAL(clicked()), this, SLOT(stopRecordingPoints()));
	calibrationPointsLayout->addWidget(_stopRecordingPointsButton, 0, 1);

	_numberAvailablePointsLabel = new QLabel(this);
	_numCalibrationPoints = 0;
	refreshAvailableCalibrationPointsLabel();
	calibrationPointsLayout->addWidget(_numberAvailablePointsLabel, 1, 0);

	calibrationPointsBox->setLayout(calibrationPointsLayout);
	_mainLayout->addWidget(calibrationPointsBox);
}

void StereoVisionCalibrationDialog::createAlgorithmParametersBox() {

	QString boxTitle("Differential Evolution Parameters");
	QGroupBox* algorithmParametersBox = new QGroupBox(boxTitle, this);
	QGridLayout* algorithmParametersLayout = new QGridLayout(algorithmParametersBox);

	_individualsSpinBox = new QDoubleSpinBox(algorithmParametersBox);
	_individualsSpinBox->setRange(10.0f, 50.0f);
	_individualsSpinBox->setSingleStep(1.0f);
	_individualsSpinBox->setDecimals(0);
	_individualsSpinBox->setValue(_individuals);
	connect(_individualsSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* individualsLabel = new QLabel("Individuals");
	algorithmParametersLayout->addWidget(individualsLabel, 0, 0);
	algorithmParametersLayout->addWidget(_individualsSpinBox, 0, 1);

	_generationsSpinBox = new QDoubleSpinBox(algorithmParametersBox);
	_generationsSpinBox->setRange(1.0f, 20000.0f);
	_generationsSpinBox->setSingleStep(10.0f);
	_generationsSpinBox->setDecimals(0);
	_generationsSpinBox->setValue(_generations);
	connect(_generationsSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* generationsLabel = new QLabel("Generations");
	algorithmParametersLayout->addWidget(generationsLabel, 0, 2);
	algorithmParametersLayout->addWidget(_generationsSpinBox, 0, 3);

	_differentialVariationSpinBox = new QDoubleSpinBox(algorithmParametersBox);
	_differentialVariationSpinBox->setRange(0.30f, 0.90f);
	_differentialVariationSpinBox->setSingleStep(0.01f);
	_differentialVariationSpinBox->setDecimals(2);
	_differentialVariationSpinBox->setValue(_differentialVariation);
	connect(_differentialVariationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* differentialVariationLabel = new QLabel("Differential Variation");
	algorithmParametersLayout->addWidget(differentialVariationLabel, 1, 0);
	algorithmParametersLayout->addWidget(_differentialVariationSpinBox, 1, 1);

	_recombinationConstantSpinBox = new QDoubleSpinBox(algorithmParametersBox);
	_recombinationConstantSpinBox->setRange(0.80f, 0.90f);
	_recombinationConstantSpinBox->setSingleStep(0.01f);
	_recombinationConstantSpinBox->setDecimals(2);
	_recombinationConstantSpinBox->setValue(_recombinationConstant);
	connect(_recombinationConstantSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* recombinationConstantLabel = new QLabel("Recombination Constant");
	algorithmParametersLayout->addWidget(recombinationConstantLabel, 1, 2);
	algorithmParametersLayout->addWidget(_recombinationConstantSpinBox, 1, 3);

	algorithmParametersBox->setLayout(algorithmParametersLayout);
	_mainLayout->addWidget(algorithmParametersBox);

}

void StereoVisionCalibrationDialog::createCameraParametersBox() {

	QString boxTitle("Camera Parameters");
	QGroupBox* cameraParametersBox = new QGroupBox(boxTitle, this);
	QGridLayout* cameraParametersLayout = new QGridLayout(cameraParametersBox);

	_maxDepthSpinBox = new QDoubleSpinBox(cameraParametersBox);
	_maxDepthSpinBox->setRange(0.1f, 10.0f);
	_maxDepthSpinBox->setSingleStep(0.1f);
	_maxDepthSpinBox->setDecimals(1);
	_maxDepthSpinBox->setValue(_maxDepth);
	connect(_maxDepthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* maxDepthLabel = new QLabel("Max Depth");
	cameraParametersLayout->addWidget(maxDepthLabel, 0, 0);
	cameraParametersLayout->addWidget(_maxDepthSpinBox, 0, 1);

	_rotationMaxSpinBox = new QDoubleSpinBox(cameraParametersBox);
	_rotationMaxSpinBox->setRange(0.0f, 360.0f);
	_rotationMaxSpinBox->setSingleStep(1.0f);
	_rotationMaxSpinBox->setDecimals(0);
	_rotationMaxSpinBox->setValue(_rotationMaxInDegrees);
	connect(_rotationMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* rotationMaxLabel = new QLabel("Max Rotation (Degrees)");
	cameraParametersLayout->addWidget(rotationMaxLabel, 0, 2);
	cameraParametersLayout->addWidget(_rotationMaxSpinBox, 0, 3);

	_focalLengthMinSpinBox = new QDoubleSpinBox(cameraParametersBox);
	_focalLengthMinSpinBox->setRange(0.0f, 1000.0f);
	_focalLengthMinSpinBox->setSingleStep(25.0f);
	_focalLengthMinSpinBox->setDecimals(0);
	_focalLengthMinSpinBox->setValue(_focalLengthMin);
	connect(_focalLengthMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* focalLengthMinLabel = new QLabel("Min Focal Length (Pixels)");
	cameraParametersLayout->addWidget(focalLengthMinLabel, 1, 0);
	cameraParametersLayout->addWidget(_focalLengthMinSpinBox, 1, 1);

	_focalLengthMaxSpinBox = new QDoubleSpinBox(cameraParametersBox);
	_focalLengthMaxSpinBox->setRange(200.0f, 30000.0f);
	_focalLengthMaxSpinBox->setSingleStep(25.0f);
	_focalLengthMaxSpinBox->setDecimals(0);
	_focalLengthMaxSpinBox->setValue(_focalLengthMax);
	connect(_focalLengthMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* focalLengthMaxLabel = new QLabel("Max Focal Length (Pixels)");
	cameraParametersLayout->addWidget(focalLengthMaxLabel, 1, 2);
	cameraParametersLayout->addWidget(_focalLengthMaxSpinBox, 1, 3);

	cameraParametersBox->setLayout(cameraParametersLayout);
	_mainLayout->addWidget(cameraParametersBox);

}

void StereoVisionCalibrationDialog::createCalibrationBox() {

	QGroupBox* calibrationBox = new QGroupBox(this);
	QHBoxLayout* calibrationBoxLayout = new QHBoxLayout(calibrationBox);

	QString calibrateButtonText("Calibrate");
	_calibrateButton = new QPushButton(calibrateButtonText, this);
	_calibrateButton->setDisabled(true);
	connect(_calibrateButton, SIGNAL(clicked()), this, SLOT(calibrate()));
	calibrationBoxLayout->addWidget(_calibrateButton);

	_progresBar = new QProgressBar(this);
	_progresBar->setMinimum(0);
	_progresBar->setMaximum(_generations - 1);
	calibrationBoxLayout->addWidget(_progresBar);

	calibrationBox->setLayout(calibrationBoxLayout);
	_mainLayout->addWidget(calibrationBox);

	_resultsText = new QTextEdit(this);
	_mainLayout->addWidget(_resultsText);

}

void StereoVisionCalibrationDialog::changeParameter(double value) {

	QObject* senderObject = sender();

	if (senderObject == _individualsSpinBox) {
		_individuals = value;
	} else if (senderObject == _generationsSpinBox) {
		_generations = value;
		_progresBar->setMaximum(value - 1);
	} else if (senderObject == _differentialVariationSpinBox) {
		_differentialVariation = value;
	} else if (senderObject == _recombinationConstantSpinBox) {
		_recombinationConstant = value;
	} else if (senderObject == _maxDepthSpinBox) {
		_maxDepth = value;
	} else if (senderObject == _rotationMaxSpinBox) {
		_rotationMaxInDegrees = value;
	} else if (senderObject == _focalLengthMaxSpinBox) {
		_focalLengthMax = value;
	} else {
		_focalLengthMin = value;
	}
}

void StereoVisionCalibrationDialog::beginRecordingPoints() {

	_calibrationPoints.clear();
	for (unsigned int i = 0; i < _camerasRef->size(); i++) {
		map<string, POI> points;
		_calibrationPoints.push_back(points);
	}

	_numCalibrationPoints = 0;
	refreshAvailableCalibrationPointsLabel();

	_beginRecordingPointsButton->setDisabled(true);
	_stopRecordingPointsButton->setEnabled(true);
	_recordPointsTimer->start(200);

}

void StereoVisionCalibrationDialog::stopRecordingPoints() {

	_beginRecordingPointsButton->setEnabled(true);
	_stopRecordingPointsButton->setDisabled(true);
	_recordPointsTimer->stop();

	if (_numCalibrationPoints > 0) {
		_calibrateButton->setEnabled(true);
	}

}

void StereoVisionCalibrationDialog::recordCalibrationPoint() {

	if (areCamerasStateValidForCalibrationPointRecording(_camerasRef)) {

		for (unsigned int i = 0; i < _camerasRef->size(); i++) {
			POI poi = _camerasRef->at(i)->getPOIs().begin()->second;

			QString semantic = QString("%1%2").arg(poi.getSemantic()).arg(_numCalibrationPoints);
			poi.setSemantic(semantic);

			(_calibrationPoints[i])[poi.getSemantic().toStdString()] = poi;

		}

		_numCalibrationPoints++;
		refreshAvailableCalibrationPointsLabel();

	}

}

void StereoVisionCalibrationDialog::refreshAvailableCalibrationPointsLabel() {

	QString availablePoints = QString("Number of Available Points to Calibrate: %1.").arg(_numCalibrationPoints);
	_numberAvailablePointsLabel->setText(availablePoints);

}

bool StereoVisionCalibrationDialog::areCamerasStateValidForCalibrationPointRecording(
		vector<AbstractCamera*>* camerasRef) {

	//--- Stereo Calibration Validation
	bool validCalibrationState = true;
	QString selectedSemantic = "";

	if (camerasRef->at(0)->getPOIs().size() > 0) {
		selectedSemantic = camerasRef->at(0)->getPOIs().begin()->second.getSemantic();
	}

	for (unsigned int i = 0; i < camerasRef->size(); i++) {

		map<string, POI> cameraPOIs = camerasRef->at(i)->getPOIs();

		//--- Just one POI with semantic
		if (cameraPOIs.size() != 1) {
			validCalibrationState = false;
			//--- Every semantic must be lexically equal
		} else if (cameraPOIs[selectedSemantic.toStdString()].getSemantic().compare(selectedSemantic) != 0) {
			validCalibrationState = false;
		}
	}

	return validCalibrationState;
}

#include "StereoVisionCalibrationDialog.moc"
