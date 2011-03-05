/*!
 * Class that implements a DummyCamera camera adapter.
 *
 * \name DummyCamera
 * \author Daniel Pacheco de Queiroz
 * \version
 * \since 01/20/2009
 */
#include "DummyCamera.h"

DummyCamera::DummyCamera(int id, int width, int height, int frameRate) :
	AbstractCamera(id, width, height, frameRate) {

	//Query camera name
	setupDevice();
	_name = "Dummy Camera";
}

DummyCamera::~DummyCamera() {

	//stopDevice();
	// Delete _frame?
}

bool DummyCamera::setupDevice() {

	setResolution(_width, _height);
	//setFrameRate(_frameRate);

	return true;

}

void DummyCamera::stopDevice() {

}

IplImage* DummyCamera::getFrame() {

	return _frame->getIplImage();
}

void DummyCamera::setResolution(int width, int height) {

	_frame = new Image(width, height, 3);
}

void DummyCamera::setFrameRate(int frameRate) {

}
