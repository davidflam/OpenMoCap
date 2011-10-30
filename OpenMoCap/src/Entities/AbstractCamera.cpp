/*!
 * Abstract class for cameras adapters.
 *
 * Base class for implementing cameras adapters.
 * See DirectShowCamera and OptiTrackCamera for sample implementations.
 *
 * \name AbstractCamera
 * \author David Lunardi Flam
 * \version
 * \since 01/19/2009
 */
#include "AbstractCamera.h"

AbstractCamera::~AbstractCamera() {

	if (_intrinsicParams != NULL) {
		cvReleaseMat(&_intrinsicParams);
	}

	if (_distortionCoefficients != NULL) {
		cvReleaseMat(&_distortionCoefficients);
	}

	if (_distortionModelX != NULL) {
		cvReleaseMat(&_distortionModelX);
	}

	if (_distortionModelY != NULL) {
		cvReleaseMat(&_distortionModelY);
	}

	if (_disparityToDepth != NULL) {
		cvReleaseMat(&_disparityToDepth);
	}

	if (_undistortedFrame != NULL) {
		cvReleaseImage(&_undistortedFrame);
	}

	delete _frame;
}

AbstractCamera::AbstractCamera(int id, int width, int height, int frameRate) :
	_id(id), _width(width), _height(height), _frameRate(frameRate), _intrinsicParams(NULL), _translation(cvPoint3D32f(0, 0, 0)),
	_rotation(cvPoint3D32f(0, 0, 0)), _distortionCoefficients(NULL),
	_distortionModelX(NULL), _distortionModelY(NULL), _disparityToDepth(NULL){

	_undistortedFrame =  cvCreateImage(cvSize(width, height), 8, 3);
	_calibrated = false;
	_cameraType = -1;
}

AbstractCamera::AbstractCamera(int id, int width, int height, int frameRate, int type) :
	_id(id), _width(width), _height(height), _frameRate(frameRate), _intrinsicParams(NULL), _translation(cvPoint3D32f(0, 0, 0)),
	_rotation(cvPoint3D32f(0, 0, 0)), _distortionCoefficients(NULL),
	_distortionModelX(NULL), _distortionModelY(NULL), _disparityToDepth(NULL) {

	_undistortedFrame =  cvCreateImage(cvSize(width, height), 8, 3);
	_calibrated = false;
	_cameraType = type;
}

QString AbstractCamera::getDistorcionCoefficientsInfo() {

	float k1 = cvmGet(_distortionCoefficients, 0, 0);
	float k2 = cvmGet(_distortionCoefficients, 1, 0);
	float p1 = cvmGet(_distortionCoefficients, 2, 0);
	float p2 = cvmGet(_distortionCoefficients, 3, 0);

	return QString("\n\nDistorcion Coefficients:\nk1 = %1 k2 = %2\np1 = %3 p2 = %4").arg(k1).arg(k2).arg(p1).arg(p2);
}

QString AbstractCamera::getIntrinsicParamsInfo() {

	QString info("");
	if (_intrinsicParams != NULL) {

		float fX = cvmGet(_intrinsicParams, 0, 0);
		float cX = cvmGet(_intrinsicParams, 0, 2);
		float fY = cvmGet(_intrinsicParams, 1, 1);
		float cY = cvmGet(_intrinsicParams, 1, 2);

		info = QString("\n\nIntrinsic Params:\nfX = %1 fY = %2\ncX = %3 cY = %4").arg(fX).arg(fY).arg(cX).arg(cY);
	}
	return info;
}

QString AbstractCamera::getExtrinsicParamsInfo() {

	return QString("\n\nExtrinsic Params:\ntX = %1 tY = %2 tZ = %3\nrX = %4 rY = %5 rZ =%6").arg(_translation.x).arg(
			_translation.y).arg(_translation.z).arg((_rotation.x * 180.0f) / CV_PI).arg((_rotation.y * 180.0f) / CV_PI).arg(
			(_rotation.z * 180.0f) / CV_PI);
}

QString AbstractCamera::getInfo() {

	QString info;

	info = QString("Camera %2 - (%1)").arg(_name.c_str()).arg(_id);
	info.append(getIntrinsicParamsInfo());
	info.append(getExtrinsicParamsInfo());
	info.append(QString("\n\n"));

	return info;
}

void AbstractCamera::resetParameters() {

	if (_intrinsicParams != NULL) {
		cvReleaseMat(&_intrinsicParams);
	}

	if (_distortionCoefficients != NULL) {
		cvReleaseMat(&_distortionCoefficients);
	}
}

Image* AbstractCamera::previewUndistort(string distortedImagePath) {

	Image* returnImages = new Image[2];
	returnImages[0] = Image(distortedImagePath.c_str());

	if (returnImages[0].getIplImage() != NULL) {
		returnImages[1] = returnImages[0].getCopy();

		//TODO Check performance related to different undistortion approaches (cvUndistortInit+cvRemap)
		cvUndistort2(returnImages[0].getIplImage(), returnImages[1].getIplImage(), _intrinsicParams,
				_distortionCoefficients);
	}

	return returnImages;
}

void AbstractCamera::insertPOI(POI& POItoInsert) {

	// TODO: Here we need to know if the tracker is kalman, to only in this case initialize kalman, how?
	// Initialze Kalman
	//KalmanFilter kalman;
	//POItoInsert.setKalman( kalman.initKalman(POItoInsert.getPredictedPosition()) );

	_POIs[POItoInsert.getSemantic().toStdString()] = POItoInsert;
}

void AbstractCamera::removePOI(string semantic) {


	//if (_POIs[semantic].getKalman() != NULL){

		//CvKalman* k = _POIs[semantic].getKalman();
		//cvReleaseKalman(&k);
	//}
	_POIs.erase(semantic);
}

CvMat* AbstractCamera::computeRtMatrix(double a, double b, double g, double tX, double tY, double tZ) {

	//--- Represent 3d rotation with euler angles

	double sinG = sin(g);
	double cosG = cos(g);
	CvMat* rZg = cvCreateMat(3, 3, CV_32FC1);
	cvSetZero(rZg);
	cvmSet(rZg, 0, 0, cosG);
	cvmSet(rZg, 0, 1, -sinG);
	cvmSet(rZg, 1, 0, sinG);
	cvmSet(rZg, 1, 1, cosG);
	cvmSet(rZg, 2, 2, 1.0f);

	double sinB = sin(b);
	double cosB = cos(b);
	CvMat* rXb = cvCreateMat(3, 3, CV_32FC1);
	cvSetZero(rXb);
	cvmSet(rXb, 0, 0, 1.0f);
	cvmSet(rXb, 1, 1, cosB);
	cvmSet(rXb, 1, 2, -sinB);
	cvmSet(rXb, 2, 1, sinB);
	cvmSet(rXb, 2, 2, cosB);

	double sinA = sin(a);
	double cosA = cos(a);
	CvMat* rZa = cvCreateMat(3, 3, CV_32FC1);
	cvSetZero(rZa);
	cvmSet(rZa, 0, 0, cosA);
	cvmSet(rZa, 0, 1, -sinA);
	cvmSet(rZa, 1, 0, sinA);
	cvmSet(rZa, 1, 1, cosA);
	cvmSet(rZa, 2, 2, 1.0f);

	CvMat* rotationMatrix = cvCreateMat(3, 3, CV_32FC1);
	cvMatMul(rZg, rXb, rotationMatrix);
	cvMatMul(rotationMatrix, rZa, rotationMatrix);

	cvReleaseMat(&rZg);
	cvReleaseMat(&rXb);
	cvReleaseMat(&rZa);

	//--- [R|T] ; First camera rotation and translation matrix
	CvMat* rtMatrix = cvCreateMat(3, 4, CV_32FC1);
	for (int i = 0; i < 3; i++) {
		cvmSet(rtMatrix, i, 0, cvmGet(rotationMatrix, i, 0));
		cvmSet(rtMatrix, i, 1, cvmGet(rotationMatrix, i, 1));
		cvmSet(rtMatrix, i, 2, cvmGet(rotationMatrix, i, 2));
	}
	cvmSet(rtMatrix, 0, 3, tX);
	cvmSet(rtMatrix, 1, 3, tY);
	cvmSet(rtMatrix, 2, 3, tZ);

	cvReleaseMat(&rotationMatrix);

	return rtMatrix;
}

CvMat* AbstractCamera::computeKMatrix(double focalLength, float pX, float pY) {

	CvMat* kMatrix = cvCreateMat(3, 3, CV_32FC1);
	cvSetZero(kMatrix);
	cvmSet(kMatrix, 0, 0, focalLength);
	cvmSet(kMatrix, 1, 1, focalLength);
	cvmSet(kMatrix, 0, 2, pX);
	cvmSet(kMatrix, 1, 2, pY);
	cvmSet(kMatrix, 2, 2, 1.0f);

	return kMatrix;
}

QString AbstractCamera::getCurrent2dPOIsInfo() {

	QString info;

	for (map<string, POI>::const_iterator it = _POIs.begin(); it != _POIs.end(); ++it) {
		POI poi = it->second;
		info.append(poi.getInfo2d());
	}

	return info;
}
