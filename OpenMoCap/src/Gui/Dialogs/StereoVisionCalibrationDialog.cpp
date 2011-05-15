/*
 * StereoVisionCalibrationDialog.cpp
 *
 *  Created on: 30/03/2009
 *      Author: David
 */

#include "StereoVisionCalibrationDialog.h"

StereoVisionCalibrationDialog::StereoVisionCalibrationDialog(CaptureController* captureController) :
	_captureControllerRef(captureController), _camerasRef(captureController->getMocap()->getCameras()),
			_modelVisualizationRef(captureController->getVisualizationRef()), _numIntCornersRow(9.0f), _numIntCornersCol(6.0f), _differentialVariation(0.84f),
			_recombinationConstant(0.9f), _maxDepth(1.5f), _rotationMaxInDegrees(90.0f), _focalLengthMax(1500.0f), _focalLengthMin(500.0f) {

	QString windowTitle("Stereo Calibration");
	setWindowTitle(windowTitle);

	_mainLayout = new QVBoxLayout();
	createCalibrationImagesBox();
	createAlgorithmParametersBox();
	createCameraParametersBox();
	createCalibrationBox();

	_mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(_mainLayout);

	_recordPointsTimer = new QTimer(this);
	connect(_recordPointsTimer, SIGNAL(timeout()), this, SLOT(recordCalibrationImage()));

}

void StereoVisionCalibrationDialog::calibrate() {

	Timer t;
	double cost = 0;

	t.start();

	DifferentialEvolutionCalibrator differentialEvolutionCalibrator(_camerasRef, _maxDepth, _rotationMaxInDegrees * (CV_PI / 180), _focalLengthMin,
			_focalLengthMax);

	vector<POI> POIs3d = differentialEvolutionCalibrator.calibrate(_numIntCornersRow, _numIntCornersCol, _differentialVariation, _recombinationConstant, cost,
			_calibrationPoints, _progresBar);

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

void StereoVisionCalibrationDialog::createCalibrationImagesBox() {

	QString boxTitle("Calibration Images");
	QGroupBox* calibrationPointsBox = new QGroupBox(boxTitle, this);
	QGridLayout* calibrationPointsLayout = new QGridLayout(calibrationPointsBox);

	_beginRecordingPointsButton = new QPushButton("Begin Recording Images", this);
	connect(_beginRecordingPointsButton, SIGNAL(clicked()), this, SLOT(beginRecordingImages()));
	calibrationPointsLayout->addWidget(_beginRecordingPointsButton, 0, 0);

	_stopRecordingPointsButton = new QPushButton("Stop Recording Images", this);
	_stopRecordingPointsButton->setDisabled(true);
	connect(_stopRecordingPointsButton, SIGNAL(clicked()), this, SLOT(stopRecordingImages()));
	calibrationPointsLayout->addWidget(_stopRecordingPointsButton, 0, 1);

	_numberAvailablePointsLabel = new QLabel(this);
	_numCalibrationPoints = 0;
	refreshAvailableCalibrationPointsLabel();
	calibrationPointsLayout->addWidget(_numberAvailablePointsLabel, 1, 0);

	calibrationPointsBox->setLayout(calibrationPointsLayout);
	_mainLayout->addWidget(calibrationPointsBox);
}

void StereoVisionCalibrationDialog::createAlgorithmParametersBox() {

	QString boxTitle("Chessboard Parameters");
	QGroupBox* algorithmParametersBox = new QGroupBox(boxTitle, this);
	QGridLayout* algorithmParametersLayout = new QGridLayout(algorithmParametersBox);

	_numIntCornersRowSpinBox = new QDoubleSpinBox(algorithmParametersBox);
	_numIntCornersRowSpinBox->setRange(3.0f, 12.0f);
	_numIntCornersRowSpinBox->setSingleStep(1.0f);
	_numIntCornersRowSpinBox->setDecimals(0);
	_numIntCornersRowSpinBox->setValue(_numIntCornersRow);
	connect(_numIntCornersRowSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* numIntCornersRowLabel = new QLabel("Number of Internal Corners (Row)");
	algorithmParametersLayout->addWidget(numIntCornersRowLabel, 0, 0);
	algorithmParametersLayout->addWidget(_numIntCornersRowSpinBox, 0, 1);

	_numIntCornersColSpinBox = new QDoubleSpinBox(algorithmParametersBox);
	_numIntCornersColSpinBox->setRange(3.0f, 12.0f);
	_numIntCornersColSpinBox->setSingleStep(1.0f);
	_numIntCornersColSpinBox->setDecimals(0);
	_numIntCornersColSpinBox->setValue(_numIntCornersCol);
	connect(_numIntCornersColSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* numIntCornersColLabel = new QLabel("Number of Internal Corners (Column)");
	algorithmParametersLayout->addWidget(numIntCornersColLabel, 0, 2);
	algorithmParametersLayout->addWidget(_numIntCornersColSpinBox, 0, 3);

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
	_progresBar->setMaximum(_numIntCornersCol - 1);
	calibrationBoxLayout->addWidget(_progresBar);

	calibrationBox->setLayout(calibrationBoxLayout);
	_mainLayout->addWidget(calibrationBox);

	_resultsText = new QTextEdit(this);
	_mainLayout->addWidget(_resultsText);

}

void StereoVisionCalibrationDialog::changeParameter(double value) {

	QObject* senderObject = sender();

	if (senderObject == _numIntCornersRowSpinBox) {
		_numIntCornersRow = value;
	} else if (senderObject == _numIntCornersColSpinBox) {
		_numIntCornersCol = value;
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

void StereoVisionCalibrationDialog::beginRecordingImages() {

	_calibrationPoints.clear();
	for (unsigned int i = 0; i < _camerasRef->size(); i++) {

		VideoController* videoControllerRef = _captureControllerRef->getVideoController(i);
		videoControllerRef->setVideoStatus(VideoStatusEnum::CALIBRATE);

		map<string, POI> points;
		_calibrationPoints.push_back(points);
	}

	_numCalibrationPoints = 0;
	refreshAvailableCalibrationPointsLabel();

	_beginRecordingPointsButton->setDisabled(true);
	_stopRecordingPointsButton->setEnabled(true);
	_recordPointsTimer->start(MSEC_BETWEEN_CALIB_IMAGES);

}

void StereoVisionCalibrationDialog::stopRecordingImages() {

	for (unsigned int i = 0; i < _camerasRef->size(); i++) {

		VideoController* videoControllerRef = _captureControllerRef->getVideoController(i);
		videoControllerRef->setVideoStatus(VideoStatusEnum::PLAY_LIVE);
	}

	_beginRecordingPointsButton->setEnabled(true);
	_stopRecordingPointsButton->setDisabled(true);
	_recordPointsTimer->stop();

	if (_numCalibrationPoints > 0) {
		_calibrateButton->setEnabled(true);
	}

}

void StereoVisionCalibrationDialog::recordCalibrationImage() {

	if (areCamerasStateValidForCalibrationPointRecording()) {

		_numCalibrationPoints++;
		refreshAvailableCalibrationPointsLabel();

	}

}

void StereoVisionCalibrationDialog::refreshAvailableCalibrationPointsLabel() {

	QString availablePoints = QString("Number of Available Points to Calibrate: %1.").arg(_numCalibrationPoints);
	_numberAvailablePointsLabel->setText(availablePoints);

}

bool StereoVisionCalibrationDialog::areCamerasStateValidForCalibrationPointRecording() {

	//--- Stereo Calibration Validation
	bool validCalibrationState = true;

	CvSize patternSize = cvSize(_numIntCornersRowSpinBox->value(), _numIntCornersColSpinBox->value());
	int numCorners = _numIntCornersRowSpinBox->value() * _numIntCornersColSpinBox->value();
	CvPoint2D32f* corners = new CvPoint2D32f[numCorners];

	for (unsigned int i = 0; i < _camerasRef->size(); i++) {
		vector<POI> newPOIs;

		IplImage* currentFrame = _camerasRef->at(i)->getFrame();

		if ( cvFindChessboardCorners(currentFrame, patternSize, corners, NULL, CV_CALIB_CB_ADAPTIVE_THRESH) ) {
			for(int k = 0; k < numCorners; k++) {
				newPOIs.push_back(POI(corners[k], true));
			}
		} else {
			validCalibrationState = false;
		}

		VideoController* videoControllerRef = _captureControllerRef->getVideoController(i);
		videoControllerRef->getPOIsImageWidgetRef()->refreshImage(currentFrame, newPOIs);

	}

	return validCalibrationState;
}

#include "StereoVisionCalibrationDialog.moc"
