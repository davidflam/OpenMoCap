/*!
 * Class that implements a DirectShow camera adapter.
 *
 * \name DirectShowCamera
 * \author David Lunardi Flam
 * \version
 * \since 01/20/2009
 */
#include "DirectShowCamera.h"

DirectShowCamera::DirectShowCamera(int id, int width, int height, int frameRate, videoInput* vInput) :
	AbstractCamera(id, width, height, frameRate) {

	_videoInput = vInput;

	//Query camera name
	setupDevice();
	_name = _videoInput->getDeviceName(_id);
}

DirectShowCamera::~DirectShowCamera() {

	stopDevice();
}

bool DirectShowCamera::setupDevice() {

	setResolution(_width, _height);
	setFrameRate(_frameRate);
	_videoInput->setAutoReconnectOnFreeze(_id, true, 3);

	return true;

}

void DirectShowCamera::stopDevice() {

	if (_videoInput->isDeviceSetup(_id)) {
		_videoInput->stopDevice(_id);
	}
}

IplImage* DirectShowCamera::getFrame() {

	if (_videoInput->isFrameNew(_id)) {
		_videoInput->getPixels(_id, _frame->getImageDataAsUnsigned(), false, true);
	}

	return _frame->getIplImage();
}

void DirectShowCamera::setResolution(int width, int height) {

	if (_videoInput->isDeviceSetup(_id)) {
		stopDevice();
	}
	_videoInput->setupDevice(_id, width, height);
	_frame = new Image(_videoInput->getWidth(_id), _videoInput->getHeight(_id), 3);
}

void DirectShowCamera::setFrameRate(int frameRate) {
	_videoInput->setIdealFramerate(_id, frameRate);
}
