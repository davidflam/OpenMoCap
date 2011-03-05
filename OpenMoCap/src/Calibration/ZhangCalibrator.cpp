/*!
 * Class that implements the Zhang calibrator.
 *
 * \name ZhangCalibrator
 * \author David Lunardi Flam
 * \version
 * \since 08/26/2008
 */
#include "ZhangCalibrator.h"

CvSize ZhangCalibrator::_patternSize = cvSize(NUM_INT_CORNERS_ROW, NUM_INT_CORNERS_COL);

void ZhangCalibrator::initializeObjectPoints() {

	_objectPoints = new CvPoint3D32f[NUM_INT_CORNERS * _validImages.size()];

	for (int i = 0; i < (int) _validImages.size(); i++) {

		int cornerIndex = 0;
		for (int y = 0; y < NUM_INT_CORNERS_ROW; y++) {
			for (int x = 0; x < NUM_INT_CORNERS_COL; x++) {
				_objectPoints[cornerIndex + (i * NUM_INT_CORNERS)] = cvPoint3D32f(x * PATTERN_SQ_WIDTH, y
						* PATTERN_SQ_WIDTH, 0);
				cornerIndex++;
			}
		}
	}
}

void ZhangCalibrator::initializePointCounts() {

	_pointCounts = new int[_validImages.size()];

	for (int i = 0; i < (int) _validImages.size(); i++) {
		_pointCounts[i] = NUM_INT_CORNERS;
	}
}

void ZhangCalibrator::fillCorners(Image& image, CvPoint2D32f* corners) {

	int cornersNum = 0;
	cvFindChessboardCorners(image.getIplImage(), _patternSize, corners, &cornersNum, 0);
	cvFindCornerSubPix(image.getIplImage(), corners, cornersNum, cvSize(4, 4), cvSize(-1, -1), cvTermCriteria(
			CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 0.1));
}

void ZhangCalibrator::initializeCorners() {

	_corners = new CvPoint2D32f[NUM_INT_CORNERS * _validImages.size()];
	int width = 0;
	int height = 0;

	for (int i = 0; i < (int) _validImages.size(); i++) {

		string fileName = _validImages[i];
		Image pattern(fileName);
		Image grayPattern = pattern.getGrayScale();
		fillCorners(grayPattern, &(_corners[i * NUM_INT_CORNERS]));
		width = pattern.getWidth();
		height = pattern.getHeight();
	}

	_patternResolution = cvSize(width, height);
}

bool ZhangCalibrator::findCorners(Image& image) {

	CvPoint2D32f* corners = new CvPoint2D32f[NUM_INT_CORNERS];
	int cornersNum = 0;

	int cornersFound = cvFindChessboardCorners(image.getIplImage(), _patternSize, corners, &cornersNum,
			CV_CALIB_CB_ADAPTIVE_THRESH);

	cvDrawChessboardCorners(image.getIplImage(), _patternSize, corners, cornersNum, cornersFound);

	//---Display Image with corners
	image.display();

	if (!cornersFound) {
		return false;
	} else {
		return true;
	}
}

void ZhangCalibrator::verifyImages() {

	_validImages.clear();

	for (int i = 0; i < (int) _imageFileNames.size(); i++) {

		string currentFileName = _imageFileNames.at(i).toStdString();
		Image pattern(currentFileName);

		if (findCorners(pattern)) {
			_validImages.push_back(currentFileName);
		}
	}
}

ZhangCalibrator::ZhangCalibrator() {

}

ZhangCalibrator::~ZhangCalibrator() {

	if (_validImages.size() > 0) {
		delete[] _objectPoints;
		delete[] _pointCounts;
		delete[] _corners;
	}
}

CalibrationResultEnum ZhangCalibrator::calibrate(AbstractCamera *camera) {

	verifyImages();

	if (_validImages.size() < 1) {
		camera->resetParameters();
		return CalibrationResultEnum::NO_VALID_IMAGE;
	}

	initializeObjectPoints();
	initializePointCounts();
	initializeCorners();

	int numberImages = _validImages.size();

	CvMat* mat2D = cvCreateMat(NUM_INT_CORNERS * numberImages, 2, CV_32FC1);
	CvMat* mat3D = cvCreateMat(NUM_INT_CORNERS * numberImages, 2, CV_32FC1);
	CvMat* pointCounts = cvCreateMat(numberImages, 1, CV_32SC1);
	CvMat* intrinsic = cvCreateMat(3, 3, CV_32FC1);
	CvMat* distCoeffs = cvCreateMat(NUM_DIST_COEFFS, 1, CV_32FC1);
	CvMat* rotation = cvCreateMat(numberImages, 3, CV_32FC1);
	CvMat* translation = cvCreateMat(numberImages, 3, CV_32FC1);

	cvSetData(mat3D, _objectPoints, sizeof(CvPoint3D32f));
	cvSetData(mat2D, _corners, sizeof(CvPoint2D32f));
	cvSetData(pointCounts, _pointCounts, sizeof(int));

	cvCalibrateCamera2(mat3D, mat2D, pointCounts, _patternResolution, intrinsic, distCoeffs, rotation, translation, 0);

	camera->setDistortionCoefficients(distCoeffs);
	camera->setIntrinsicParams(intrinsic);

	//---Sets translation vector
	CvPoint3D32f translationVector;
	translationVector.x = cvmGet(translation, 0, 0);
	translationVector.y = cvmGet(translation, 0, 1);
	translationVector.z = cvmGet(translation, 0, 2);
	camera->setTranslation(translationVector);

	//---Sets rotation vector
	CvPoint3D32f rotationVector;
	rotationVector.x = cvmGet(rotation, 0, 0);
	rotationVector.y = cvmGet(rotation, 0, 1);
	rotationVector.z = cvmGet(rotation, 0, 2);
	camera->setRotation(rotationVector);

	//---Clean Heap
	cvReleaseMat(&mat2D);
	cvReleaseMat(&mat3D);
	cvReleaseMat(&pointCounts);
	cvReleaseMat(&rotation);
	cvReleaseMat(&translation);

	return CalibrationResultEnum::SUCCESS;
}

CalibrationResultEnum ZhangCalibrator::calibrateExtrinsicParameters(AbstractCamera *camera) {

	verifyImages();

	if (_validImages.size() < 1) {
		camera->resetParameters();
		return CalibrationResultEnum::NO_VALID_IMAGE;
	}

	int numberImages = _validImages.size();

	//---Object points
	initializeObjectPoints();
	//---Image points
	initializeCorners();

	CvMat* mat2D = cvCreateMat(NUM_INT_CORNERS * numberImages, 2, CV_32FC1);
	cvSetData(mat2D, _corners, sizeof(CvPoint2D32f));

	CvMat* mat3D = cvCreateMat(NUM_INT_CORNERS * numberImages, 2, CV_32FC1);
	cvSetData(mat3D, _objectPoints, sizeof(CvPoint3D32f));

	CvMat* rotation = cvCreateMat(numberImages, 3, CV_32FC1);
	CvMat* translation = cvCreateMat(numberImages, 3, CV_32FC1);

	cvFindExtrinsicCameraParams2(mat3D, mat2D, camera->getIntrinsicParams(), camera->getDistortionCoefficients(),
			rotation, translation);

	//---Sets translation vector
	CvPoint3D32f translationVector;
	translationVector.x = cvmGet(translation, 0, 0);
	translationVector.y = cvmGet(translation, 0, 1);
	translationVector.z = cvmGet(translation, 0, 2);
	camera->setTranslation(translationVector);

	//---Sets rotation vector
	CvPoint3D32f rotationVector;
	rotationVector.x = cvmGet(rotation, 0, 0);
	rotationVector.y = cvmGet(rotation, 0, 1);
	rotationVector.z = cvmGet(rotation, 0, 2);
	camera->setRotation(rotationVector);

	//---Clean Heap
	cvReleaseMat(&mat2D);
	cvReleaseMat(&mat3D);
	cvReleaseMat(&rotation);
	cvReleaseMat(&translation);

	return CalibrationResultEnum::SUCCESS;
}
