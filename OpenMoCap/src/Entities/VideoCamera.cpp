/*!
 * Class that implements a VideoCamera camera adapter.
 *
 * \name VideoCamera
 * \author André Taiar Marinho Oliveira
 * \version
 * \since 01/22/2010
 */
#include "VideoCamera.h"

VideoCamera::VideoCamera(int id, int width, int height, int frameRate) :
	AbstractCamera(id, width, height, frameRate, VIDEO) {

	//Query camera name
	setupDevice();
	_name = "Video Camera";
	stringstream nome;
	nome << _name << " " << id;
	_name = nome.str();

	_gotVideo = false;
	_isPlaying = false;
	_vidActualPos = .0;
}

VideoCamera::VideoCamera(int id, int width, int height, int frameRate,
		QString videoLocation) :
	AbstractCamera(id, width, height, frameRate, VIDEO) {

	//Query camera name
	setupDevice();
	_name = "Video Camera";
	stringstream nome;
	nome << _name << " " << id;
	_name = nome.str();

	this->_vidCapture = cvCreateFileCapture(
			(const char *) videoLocation.toLatin1());
	cvSetCaptureProperty(this->_vidCapture, CV_CAP_PROP_POS_AVI_RATIO, 1.);
	this->_vidFrameNumbers = (long) cvGetCaptureProperty(this->_vidCapture,
			CV_CAP_PROP_POS_FRAMES);
	cvSetCaptureProperty(this->_vidCapture, CV_CAP_PROP_POS_AVI_RATIO, 0.);
	this->_vidActualFrameNumber = 0;

	_gotVideo = true;
	_isPlaying = true;
	_vidActualPos = .0;
}

VideoCamera::~VideoCamera() {

	//stopDevice();
	// Delete _frame?
}

bool VideoCamera::setupDevice() {

	setResolution(_width, _height);
	//setFrameRate(_frameRate);

	return true;

}

void VideoCamera::stopDevice() {

}

IplImage* VideoCamera::getFrame() {
	if (_gotVideo) {
		if (_isPlaying) {
			if (this->_vidActualFrameNumber == this->_vidFrameNumbers) {
				cvSetCaptureProperty(this->_vidCapture,
						CV_CAP_PROP_POS_AVI_RATIO, 0.);
				this->_vidActualFrameNumber = 0;
			}
			this->_vidFrame = cvQueryFrame(this->_vidCapture);
			this->_vidActualFrameNumber += 1;
			return this->_vidFrame;
		} else {
			this->_vidFrame = cvQueryFrame(this->_vidCapture);
			cvSetCaptureProperty(this->_vidCapture, CV_CAP_PROP_POS_AVI_RATIO,
					_vidActualPos);
			return this->_vidFrame;
		}
	} else {
		return _frame->getIplImage();
	}
}

void VideoCamera::setResolution(int width, int height) {

	_frame = new Image(width, height, 3);
}

void VideoCamera::setSourceVideo(QString videoLocation) {
	this->_vidCapture = cvCreateFileCapture(
			(const char *) videoLocation.toLatin1());
	cvSetCaptureProperty(this->_vidCapture, CV_CAP_PROP_POS_AVI_RATIO, 1.);
	this->_vidFrameNumbers = (long) cvGetCaptureProperty(this->_vidCapture,
			CV_CAP_PROP_POS_FRAMES);
	cvSetCaptureProperty(this->_vidCapture, CV_CAP_PROP_POS_AVI_RATIO, 0.);
	this->_vidActualFrameNumber = 0;

	_gotVideo = true;
	_isPlaying = true;
	/*
	// Clear the POIs
	map<string, POI> POIs = getPOIs();
	for (map<string, POI>::iterator it = POIs.begin(); it != POIs.end(); it++) {
		it->second.get;
	}*/
}

void VideoCamera::resetVideo() {
	if (_gotVideo) {
		cvSetCaptureProperty(this->_vidCapture, CV_CAP_PROP_POS_AVI_RATIO, 0.);
		this->_vidActualFrameNumber = 0;
		_isPlaying = true;
	}
}

void VideoCamera::stopVideo() {
	if (_isPlaying) {
		_vidActualPos = (float) cvGetCaptureProperty(this->_vidCapture, CV_CAP_PROP_POS_AVI_RATIO);
		_isPlaying = false;
	}
}

void VideoCamera::playVideo() {
	if(!_isPlaying)
		_isPlaying = true;
}

void VideoCamera::setFrameRate(int frameRate) {

}
