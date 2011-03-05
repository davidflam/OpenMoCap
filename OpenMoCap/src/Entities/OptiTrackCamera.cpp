/*!
 * Class that implements an OptiTrack camera adapter.
 *
 * \name OptiTrackCamera
 * \author David Lunardi Flam
 * \version
 * \since 01/20/2009
 */
#include "OptiTrackCamera.h"

INPCameraCollection* OptiTrackCamera::CAMERA_COLLECTION;

OptiTrackCamera::OptiTrackCamera(int id, int width, int height, int frameRate, int cameraCollectionIndex) :
	AbstractCamera(id, width, height, frameRate), _camera(NULL), _NPFrame(NULL), _cameraCollectionIndex(
			cameraCollectionIndex) {

	//TODO Dynamic load OptiTrack library
	CAMERA_COLLECTION->Item(cameraCollectionIndex, &_camera);
	setupDevice();
	setResolution(_width, _height);
	setFrameRate(_frameRate);

	/*Start camera capture*/
	_camera->Start();
}

OptiTrackCamera::~OptiTrackCamera() {

	_camera->Release();
}

void OptiTrackCamera::initializeOptiTrackCameras(vector<AbstractCamera*>* camerasRef, int width, int height,
		int frameRate) {

	//--- Create CamerCollection Instance
	CoCreateInstance(CLSID_NPCameraCollection, NULL, CLSCTX_ALL,
	IID_INPCameraCollection, (void **) &CAMERA_COLLECTION);

	CAMERA_COLLECTION->Enum();
	long numOptiCameras = 0;
	CAMERA_COLLECTION->get_Count(&numOptiCameras);

//	cerr << "OptiTrackCameras Detected: " << numOptiCameras << endl;

	for (int i=0; i < numOptiCameras; i++) {
		OptiTrackCamera *camera = new OptiTrackCamera(camerasRef->size(), width, height, frameRate, i);
		camerasRef->push_back(camera);
	}
}

void OptiTrackCamera::releaseOptiTrackCameras() {

	CAMERA_COLLECTION->Release();
	/*
	 * We don't need to call CoUninitialize since videoInput will call it already.
	 */
}

bool OptiTrackCamera::setupDevice() {

	if (_camera != NULL) {

		_camera->Open();

		/*Grayscale Video*/
		VARIANT grayVideo;
		grayVideo.vt = VT_I4;
		//---Grayscale video mode
		grayVideo.intVal = 1;
		//---Objects in camera video mode
//		grayVideo.intVal = 2;
		_camera->SetOption(NP_OPTION_VIDEO_TYPE, grayVideo);

		long serialNumber = 0;
		_camera->get_SerialNumber(&serialNumber);

		//Simple name, no model specified
		_name = "Optitrack Camera ";
		_name.append(Utils::toString(serialNumber));

	}

	return true;
}

void OptiTrackCamera::stopDevice() {

	_camera->Stop();
	_camera->Close();
}

IplImage* OptiTrackCamera::getFrame() {

	_camera->GetFrame(MAXLONG, &_NPFrame);

	if (_NPFrame != NULL && _NPFrame != 0) {

		_camera->GetFrameImage(_NPFrame, _width, _height, _frame->getWidthStep(), _frame->getBands() * 8,
				(byte*) _frame->getImageDataAsUnsigned());
		_NPFrame->Release();
	}

	return _frame->getIplImage();
}

void OptiTrackCamera::setResolution(int width, int height) {

	/*Grayscale Decimation*/
	VARIANT decimation;
	decimation.vt = VT_I4;

	if (width <= 160) {
		decimation.intVal = 4;
	} else if (width <= 320) {
		decimation.intVal = 2;
	} else {
		decimation.intVal = 0;
	}

	_camera->SetOption(NP_OPTION_GRAYSCALE_DECIMATION, decimation);

	_frame = new Image(_width, _height, 3);
}

void OptiTrackCamera::setFrameRate(int frameRate) {

	/*FrameRate Decimation*/
	VARIANT frameRateOption;
	frameRateOption.vt = VT_I4;

	if (frameRate <= 25) {
		frameRateOption.intVal = 25;
	} else if (frameRate <= 50) {
		frameRateOption.intVal = 50;
	} else {
		frameRateOption.intVal = 100;
	}

	_camera->SetOption(NP_OPTION_FRAME_RATE, frameRateOption);
}
