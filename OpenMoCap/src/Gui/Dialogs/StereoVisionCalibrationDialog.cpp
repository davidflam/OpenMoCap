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
    double rmsError = cvStereoCalibrate( &_objectPoints, &_imagePoints1, &_imagePoints2, &_pointCounts, M1, D1, M2, D2
    		, cvSize(imageWidth, imageHeight)
    		, R, T, E, F
    		, cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5)
    		, CV_CALIB_SAME_FOCAL_LENGTH + CV_CALIB_ZERO_TANGENT_DIST
    );

	float processingTime = t.stop();

	// CALIBRATION QUALITY CHECK
	// because the output fundamental matrix implicitly
	// includes all the output information,
	// we can check the quality of calibration using the
	// epipolar geometry constraint: m2^t*F*m1=0

	vector<CvPoint3D32f> lines[2];
	imagePoints[0].resize(numPoints);
	imagePoints[1].resize(numPoints);
	_imagePoints1 = cvMat(1, numPoints, CV_32FC2, &imagePoints[0][0] );
	_imagePoints2 = cvMat(1, numPoints, CV_32FC2, &imagePoints[1][0] );
	lines[0].resize(numPoints);
	lines[1].resize(numPoints);
	CvMat L1 = cvMat(1, numPoints, CV_32FC3, &lines[0][0]);
	CvMat L2 = cvMat(1, numPoints, CV_32FC3, &lines[1][0]);
	//Always work in undistorted space
	cvUndistortPoints( &_imagePoints1, &_imagePoints1, M1, D1, 0, M1 );
	cvUndistortPoints( &_imagePoints2, &_imagePoints2, M2, D2, 0, M2 );
	cvComputeCorrespondEpilines( &_imagePoints1, 1, F, &L1 );
	cvComputeCorrespondEpilines( &_imagePoints2, 2, F, &L2 );
	double reprojectionError = 0;

	for( int i = 0; i < numPoints; i++ ) {
		double err = fabs(imagePoints[0][i].x*lines[1][i].x
				   + imagePoints[0][i].y*lines[1][i].y + lines[1][i].z)
				   + fabs(imagePoints[1][i].x*lines[0][i].x
				   + imagePoints[1][i].y*lines[0][i].y + lines[0][i].z);
		reprojectionError += err;
	}
	reprojectionError = reprojectionError /( numSamples * numCorners );

	//--- Computes disparity-to-depth mapping matrix Q
	//--- BOUGUET'S METHOD

	double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
    double Q[4][4];

    CvMat _R1 = cvMat(3, 3, CV_64F, R1);
    CvMat _R2 = cvMat(3, 3, CV_64F, R2);
    CvMat _P1 = cvMat(3, 4, CV_64F, P1);
    CvMat _P2 = cvMat(3, 4, CV_64F, P2);
    CvMat _Q = cvMat(4,4, CV_64F, Q);

    cvStereoRectify(M1, M2, D1, D2, cvSize(imageWidth, imageHeight), R, T, &_R1, &_R2, &_P1, &_P2, &_Q, 0/*CV_CALIB_ZERO_DISPARITY*/ );

    CvMat* mx1 = cvCreateMat( imageHeight, imageWidth, CV_32F );
    CvMat* my1 = cvCreateMat( imageHeight, imageWidth, CV_32F );
    CvMat* mx2 = cvCreateMat( imageHeight, imageWidth, CV_32F );
    CvMat* my2 = cvCreateMat( imageHeight, imageWidth, CV_32F );

    //--- Precompute maps for cvRemap()
    cvInitUndistortRectifyMap(M1, D1, &_R1, &_P1, mx1, my1);
    cvInitUndistortRectifyMap(M2, D2, &_R2, &_P2, mx2, my2);

    cvSave("Q.xml", &_Q);
	_camerasRef->at(0)->setDisparityToDepth(_Q);
	_camerasRef->at(1)->setDisparityToDepth(_Q);

    cvSave("mx1.xml", mx1);
	_camerasRef->at(0)->setDistortionModelX(*mx1);
    cvSave("my1.xml", my1);
	_camerasRef->at(0)->setDistortionModelY(*my1);

    cvSave("mx2.xml", mx2);
	_camerasRef->at(1)->setDistortionModelX(*mx2);
    cvSave("my2.xml", my2);
	_camerasRef->at(1)->setDistortionModelY(*my2);

	QString results = QString("Calibration ended in %1s\nBack projection error = %2px\n").arg(processingTime).arg(reprojectionError);
	_resultsText->setText(results);
	logDEBUG("Calibration ended in %.2fs. Back projection error = %.2fpx", processingTime, reprojectionError);

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

	int numSamples = _calibrationPoints.size() / (_numIntCornersCol * _numIntCornersRow  * 2);
	QString availablePoints = QString("%1 image(s) available for calibration. Total of %2 points.").arg(numSamples).arg(_calibrationPoints.size());
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
