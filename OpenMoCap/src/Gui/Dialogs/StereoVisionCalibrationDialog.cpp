/*
 * StereoVisionCalibrationDialog.cpp
 *
 *  Created on: 30/03/2009
 *      Author: David
 */

#include "StereoVisionCalibrationDialog.h"

StereoVisionCalibrationDialog::StereoVisionCalibrationDialog(CaptureController* captureController) :
	_captureControllerRef(captureController), _camerasRef(captureController->getMocap()->getCameras())
	,_modelVisualizationRef(captureController->getVisualizationRef()), _numIntCornersRow(9.0f), _numIntCornersCol(6.0f)
	, _calibrationPatternWidth(2.5f), _calibrationPatternHeight(2.5f) {

	QString windowTitle("Stereo Calibration");
	setWindowTitle(windowTitle);

	_mainLayout = new QVBoxLayout();
	createCalibrationImagesBox();
	createAlgorithmParametersBox();
	createCalibrationBox();

	_mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(_mainLayout);

	_recordPointsTimer = new QTimer(this);
	connect(_recordPointsTimer, SIGNAL(timeout()), this, SLOT(recordCalibrationImage()));

}

void StereoVisionCalibrationDialog::calibrate() {

	Timer t;
	t.start();

	int numCameras =  2;
	int numCorners = (int)_numIntCornersCol * (int)_numIntCornersRow;
	int numSamples = _calibrationPoints.size() / ( numCameras * numCorners);
	int numPoints = numCorners * numSamples;
	int imageWidth = _captureControllerRef->getMocap()->getCameraResolutionWidth();
	int imageHeight = _captureControllerRef->getMocap()->getCameraResolutionHeight();

	logDEBUG("Calibration started with %d sample(s).", numSamples);

    CvMat* M1 = cvCreateMat(3, 3, CV_64F);
    CvMat* M2 = cvCreateMat(3, 3, CV_64F);
    CvMat* D1 = cvCreateMat(1, 5, CV_64F);
    CvMat* D2 = cvCreateMat(1, 5, CV_64F);
    CvMat* R = cvCreateMat(3, 3, CV_64F);
    CvMat* T = cvCreateMat(3, 1, CV_64F);
    CvMat* E = cvCreateMat(3, 3, CV_64F);
    CvMat* F = cvCreateMat(3, 3, CV_64F);

    vector<CvPoint3D32f> objectPoints (numPoints);

	//-- Object Points
    for(int k = 0; k < numSamples; k++) {

        for(int i = 0; i < (int)_numIntCornersRow; i++ ) {
        	 for(int j = 0; j < (int)_numIntCornersCol; j++ ) {

        		 CvPoint3D32f objectPoint = cvPoint3D32f(i * _calibrationPatternWidth, j * _calibrationPatternHeight, 0);
        		 objectPoints[(k * numCorners) + (i * (int)_numIntCornersCol) + j] = objectPoint;

        	 }
        }
    }

    //--- Point Counts
	int* pointCounts = new int[numSamples];
	for (int i = 0; i < numSamples; i++) {
		pointCounts[i] = numCorners;
	}

    vector<CvPoint2D32f> imagePoints[numCameras];
    int currentCamera = -1;

	for (unsigned int i = 0; i < _calibrationPoints.size(); i++) {

		if (i % numCorners == 0) {
			if (++currentCamera >= numCameras) {
				currentCamera = 0;
			}
		}

		POI poi = _calibrationPoints[i];
		imagePoints[currentCamera].push_back(poi.getCoordinates2d());
	}


	logDEBUG("Before Calibration");

	CvMat _objectPoints = cvMat(1, numPoints, CV_32FC3, &objectPoints[0]);
	Debug::printCvMat(&_objectPoints, "-==Object Points==-");

	CvMat _imagePoints1 = cvMat(1, numPoints, CV_32FC2, &imagePoints[0][0]);
	Debug::printCvMat(&_imagePoints1, "-==Image Points 1==-");

	CvMat _imagePoints2 = cvMat(1, numPoints, CV_32FC2, &imagePoints[1][0]);
	Debug::printCvMat(&_imagePoints2, "-==Image Points 2==-");

	CvMat _pointCounts = cvMat(1, numSamples, CV_32SC1, pointCounts);
	Debug::printCvMat(&_pointCounts, "-==Point Counts==-");

	cvSetIdentity(M1);
	cvSetIdentity(M2);
	cvZero(D1);
	cvZero(D2);

    //CALIBRATE THE STEREO CAMERAS
    double reprojectionError = cvStereoCalibrate( &_objectPoints, &_imagePoints1, &_imagePoints2, &_pointCounts, M1, D1, M2, D2
    		, cvSize(imageWidth, imageHeight)
    		, R, T, E, F
    		, cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5)
    		, CV_CALIB_SAME_FOCAL_LENGTH + CV_CALIB_ZERO_TANGENT_DIST
    );

	float processingTime = t.stop();
	QString results = QString("Calibration ended in %1s\nBack projection error = %2\n").arg(processingTime).arg(reprojectionError);
	_resultsText->setText(results);
	logDEBUG("Calibration ended in %.2fs. Back projection error = %.2fpx", processingTime, reprojectionError);

	Debug::printCvMat(M1, "-==M1==-");
	_camerasRef->at(0)->setIntrinsicParams(M1);
	Debug::printCvMat(M2, "-==M2==-");
	_camerasRef->at(1)->setIntrinsicParams(M2);
	Debug::printCvMat(D1, "-==D1==-");
	_camerasRef->at(0)->setDistortionCoefficients(D1);
	Debug::printCvMat(D2, "-==D2==-");
	_camerasRef->at(1)->setDistortionCoefficients(D2);
	Debug::printCvMat(R, "-==R==-");
	Debug::printCvMat(T, "-==T==-");
	Debug::printCvMat(E, "-==E==-");
	Debug::printCvMat(F, "-==F==-");

	for (unsigned int i = 0; i < _camerasRef->size(); i++) {
		_resultsText->append((_camerasRef->at(i))->getInfo());
	}

}

void StereoVisionCalibrationDialog::calibrateExtrinsic() {

	calibrate();

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

	_calibrationPatternWidthSpinBox = new QDoubleSpinBox(algorithmParametersBox);
	_calibrationPatternWidthSpinBox->setRange(1.0f, 5.0f);
	_calibrationPatternWidthSpinBox->setSingleStep(0.1f);
	_calibrationPatternWidthSpinBox->setDecimals(1);
	_calibrationPatternWidthSpinBox->setValue(_calibrationPatternWidth);
	connect(_calibrationPatternWidthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* calibrationPatternWidthLabel = new QLabel("Calibration Pattern Width (cm)");
	algorithmParametersLayout->addWidget(calibrationPatternWidthLabel, 1, 0);
	algorithmParametersLayout->addWidget(_calibrationPatternWidthSpinBox, 1, 1);

	_calibrationPatternHeightSpinBox = new QDoubleSpinBox(algorithmParametersBox);
	_calibrationPatternHeightSpinBox->setRange(1.0f, 5.0f);
	_calibrationPatternHeightSpinBox->setSingleStep(0.1f);
	_calibrationPatternHeightSpinBox->setDecimals(1);
	_calibrationPatternHeightSpinBox->setValue(_calibrationPatternHeight);
	connect(_calibrationPatternHeightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
	QLabel* calibrationPatternHeightLabel = new QLabel("Calibration Pattern Height (cm)");
	algorithmParametersLayout->addWidget(calibrationPatternHeightLabel, 1, 2);
	algorithmParametersLayout->addWidget(_calibrationPatternHeightSpinBox, 1, 3);

	algorithmParametersBox->setLayout(algorithmParametersLayout);
	_mainLayout->addWidget(algorithmParametersBox);

}

void StereoVisionCalibrationDialog::createCalibrationBox() {

	QGroupBox* calibrationBox = new QGroupBox(this);
	QHBoxLayout* calibrationBoxLayout = new QHBoxLayout(calibrationBox);

	_calibrateButton = new QPushButton(QString("Calibrate"), this);
	_calibrateButton->setDisabled(true);
	connect(_calibrateButton, SIGNAL(clicked()), this, SLOT(calibrate()));
	calibrationBoxLayout->addWidget(_calibrateButton);

	_calibrateExtrinsicButton = new QPushButton(QString("Calibrate Extrinsic"), this);
	_calibrateExtrinsicButton->setDisabled(true);
	connect(_calibrateExtrinsicButton, SIGNAL(clicked()), this, SLOT(calibrateExtrinsic()));
	calibrationBoxLayout->addWidget(_calibrateExtrinsicButton);

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
	} else if (senderObject == _calibrationPatternWidthSpinBox) {
		_calibrationPatternWidth = value;
	} else if (senderObject == _calibrationPatternHeightSpinBox) {
		_calibrationPatternHeight = value;
	}
}

void StereoVisionCalibrationDialog::beginRecordingImages() {

	_calibrationPoints.clear();
	refreshAvailableCalibrationPointsLabel();

	for (unsigned int i = 0; i < _camerasRef->size(); i++) {

		VideoController* videoControllerRef = _captureControllerRef->getVideoController(i);
		videoControllerRef->setVideoStatus(VideoStatusEnum::CALIBRATE);
	}

	_beginRecordingPointsButton->setDisabled(true);
	_stopRecordingPointsButton->setEnabled(true);
	_recordPointsTimer->start(MSEC_BETWEEN_CALIB_IMAGES);

}

void StereoVisionCalibrationDialog::stopRecordingImages() {

	_recordPointsTimer->stop();
	_beginRecordingPointsButton->setEnabled(true);
	_stopRecordingPointsButton->setDisabled(true);

	for (unsigned int i = 0; i < _camerasRef->size(); i++) {

		VideoController* videoControllerRef = _captureControllerRef->getVideoController(i);
		videoControllerRef->setVideoStatus(VideoStatusEnum::PLAY_LIVE);
	}

	if (_calibrationPoints.size() > 0) {
		_calibrateButton->setEnabled(true);
		_calibrateExtrinsicButton->setEnabled(true);
	}

}

void StereoVisionCalibrationDialog::recordCalibrationImage() {

	if (foundValidCornersForStereoCalibration()) {

		refreshAvailableCalibrationPointsLabel();

	}

}

void StereoVisionCalibrationDialog::refreshAvailableCalibrationPointsLabel() {

	QString availablePoints = QString("Number of Available Points to Calibrate: %1.").arg(_calibrationPoints.size());
	_numberAvailablePointsLabel->setText(availablePoints);

}

bool StereoVisionCalibrationDialog::foundValidCornersForStereoCalibration() {

	//--- Stereo Calibration Validation
	bool foundValidCornersForStereoCalibration = true;

	CvSize patternSize = cvSize(_numIntCornersRow, _numIntCornersCol);
	int numCorners = _numIntCornersRow * _numIntCornersCol;
	CvPoint2D32f* corners = new CvPoint2D32f[numCorners];

	for (unsigned int i = 0; i < _camerasRef->size(); i++) {
		int numDetectedCorners = 0;
		vector<POI> newPOIs;

		IplImage* currentFrame = _camerasRef->at(i)->getFrame();
		IplImage* grayImage;

		if (currentFrame->nChannels == 3) {
			grayImage = cvCreateImage(cvSize(currentFrame->width, currentFrame->height), IPL_DEPTH_8U, 1);
			cvCvtColor(currentFrame, grayImage, CV_BGR2GRAY);
		} else {
			grayImage = currentFrame;
		}

		if ( cvFindChessboardCorners(grayImage, patternSize, corners, &numDetectedCorners, CV_CALIB_CB_ADAPTIVE_THRESH) && numDetectedCorners == numCorners ) {

			cvFindCornerSubPix(grayImage, corners, numCorners, cvSize(5, 5), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 0.1));

			for(int k = 0; k < numCorners; k++) {
				POI calibrationPoint(corners[k], true);
				newPOIs.push_back(calibrationPoint);
				_calibrationPoints.push_back(calibrationPoint);
			}
		} else {
			foundValidCornersForStereoCalibration = false;
		}

		VideoController* videoControllerRef = _captureControllerRef->getVideoController(i);
		videoControllerRef->getPOIsImageWidgetRef()->refreshImage(currentFrame, newPOIs);

		//---Release created grayImage
		if (currentFrame->nChannels == 3) {
			cvReleaseImage(&grayImage);
		}

	}

	return foundValidCornersForStereoCalibration;
}

#include "StereoVisionCalibrationDialog.moc"
