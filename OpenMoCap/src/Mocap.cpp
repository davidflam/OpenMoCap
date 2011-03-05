/*!
 * The main class. Has the functions that represent the initial configurations and starts the program execution.
 *
 * \name OpenMoCap
 * \author David Lunardi Flam
 * \version
 * \since 06/23/2008
 */
#include "Mocap.h"

AbstractCalibrator* Mocap::getCalibrationAlgorithm() {

	AbstractCalibrator* calibrator = NULL;

	if (_calibrationAlgorithmType == CalibrationAlgorithmEnum::ZHANG) {
		calibrator = new ZhangCalibrator();
	} else {
		logEMERGENCY("Unrecognized Calibration Algorithm Type.");
		exit(1);
	}

	//--- Delegate memory management to object using it
	return calibrator;
}

AbstractPOIFinder* Mocap::getPOIFinderAlgorithm() {

	AbstractPOIFinder* poiFinder = NULL;

	if (_POIFinderAlgorithmType == POIFinderAlgorithmEnum::BLOB_EXTRACTOR_6_CONNECTIVITY) {
		poiFinder = new BlobExtractor();
	} else {
		logEMERGENCY("Unrecognized POI Finder Algorithm Type");
		exit(1);
	}

	//--- Delegate memory management to object using it
	return poiFinder;
}

AbstractTracker* Mocap::getTrackingAlgorithm() {

	AbstractTracker* tracker = NULL;

	if (_trackingAlgorithmType == TrackingAlgorithmEnum::ALPHA_BETA_GAMMA_FILTER) {
		tracker = new AlphaBetaGammaFilter();
	}
	else if (_trackingAlgorithmType == TrackingAlgorithmEnum::KALMAN_FILTER) {
			tracker = new KalmanFilter();
	}
	else if (_trackingAlgorithmType == TrackingAlgorithmEnum::CONDENSATION) {
			tracker = new Condensation();
	}
	else {
		logEMERGENCY("Unrecognized Tracking Algorithm Type");
	}

	//--- Delegate memory management to object using it
	return tracker;

}

AbstractReconstructor* Mocap::getReconstructionAlgorithm() {

	AbstractReconstructor* reconstructor = NULL;

	if (_reconstructionAlgorithmType == ReconstructionAlgorithmEnum::BASIC_RECONSTRUCTOR) {
		reconstructor = new BasicReconstructor();
	} else {
		logEMERGENCY("Unrecognized Reconstruction Algorithm Type");
		exit(1);
	}

	//--- Delegate memory management to object using it
	return reconstructor;

}

AbstractOutputWriter* Mocap::getOutputWriterAlgorithm() {

	AbstractOutputWriter* outputWriter = NULL;

	//TODO Implement enumeration interface for output writer algorithms and switching logic
	outputWriter = new TrcFileWriter(_cameraFrameRate);

	//--- Delegate memory management to object using it
	return outputWriter;
}

void Mocap::addDefaultCameraProperties() {
	_cameraResolutionWidth = 640;
	_cameraResolutionHeight = 480;
	_cameraFrameRate = 25;
	_videoCamerasNumber = 0;
}

void Mocap::initializeCameraProperties() {

	_cameraResolutionWidth = 0;
	_cameraResolutionHeight = 0;
	_cameraFrameRate = 0;
	_videoCamerasNumber = 0;

	ifstream cameraPropertiesFile;
	cameraPropertiesFile.open("Resources/camera/cameraProperties.ini");

	if (cameraPropertiesFile.is_open()) {

		string property, value;

		while (!cameraPropertiesFile.eof()) {

			cameraPropertiesFile>>property>>value;

			if(!property.compare("WIDTH"))
				_cameraResolutionWidth = atoi(value.c_str());

			else if(!property.compare("HEIGHT"))
				_cameraResolutionHeight = atoi(value.c_str());

			else if(!property.compare("FRAMERATE"))
				_cameraFrameRate = atoi(value.c_str());

			else if(!property.compare("VIDEOCAMERASN"))
				_videoCamerasNumber = atoi(value.c_str());
		}

		cameraPropertiesFile.close();

	} else {

		addDefaultCameraProperties();

	}
}

void Mocap::initializeCameras() {

	/**
	 * DirectShowCameras running
	 */
	int i = 0;
	int nCameras = _videoInput.listDevices(true);
	for (; i < nCameras; i++) {
		DirectShowCamera* camera = new DirectShowCamera(i, _cameraResolutionWidth, _cameraResolutionHeight,
				_cameraFrameRate, &_videoInput);
		_cameras.push_back(camera);
	}

	/**
	 * VideoCameras
	 **/
	for (int j = 0; j < _videoCamerasNumber; j += 1) {
		VideoCamera* camera = new VideoCamera(i, _cameraResolutionWidth, _cameraResolutionHeight,
				_cameraFrameRate);
		_cameras.push_back(camera);
		i += 1;
	}

	/**
	 * OptiTrackCameras running
	 */
    OptiTrackCamera::initializeOptiTrackCameras(&_cameras, _cameraResolutionWidth, _cameraResolutionHeight,
			_cameraFrameRate);
}

void Mocap::releaseCameras() {

	for (int i = 0; i < (int) _cameras.size(); i++) {
		AbstractCamera* camera = _cameras.at(i);
		delete camera;
	}

	_cameras.clear();

	OptiTrackCamera::releaseOptiTrackCameras();
}

void Mocap::initializeSkeleton() {

	BvhReader bvhReader("Resources/skeleton/default.bvh");
	_skeleton = bvhReader.getSkeleton();
}

Mocap::Mocap(int argc, char* argv[]) :
	_calibrationAlgorithmType(CalibrationAlgorithmEnum::ZHANG), _POIFinderAlgorithmType(
			POIFinderAlgorithmEnum::BLOB_EXTRACTOR_6_CONNECTIVITY), _trackingAlgorithmType(
			TrackingAlgorithmEnum::KALMAN_FILTER), _reconstructionAlgorithmType(
			ReconstructionAlgorithmEnum::BASIC_RECONSTRUCTOR) {

	// ALPHA_BETA_GAMMA_FILTER
	// KALMAN_FILTER
	// CONDENSATION
	//	Q_INIT_RESOURCE(application);
	_mocapApp = new QApplication(argc, argv);

	initializeCameraProperties();
	initializeCameras();
	initializeSkeleton();
}

int Mocap::run() {

	MainWindow mainWindow(this);
	mainWindow.showMaximized();

	return _mocapApp->exec();
}

Mocap::~Mocap() {

	releaseCameras();
	POI::fileOut.close(); // Debug Pacheco
	delete _mocapApp;
}

void signalHandler(int code) {

	switch (code) {

	case SIGSEGV:
		logERROR("Segmentation violation");
		exit(-1);
		break;
	}

}

int main(int argc, char* argv[]) {

	//--- Register signals
	signal(SIGSEGV, signalHandler);

//	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	/*
	 * OpenCv 1.1 initializes COM with incompatible thread model with
	 * Optitrack SDK
	 */
	HRESULT res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	/**
	 * Define the message pattern
	 */
	Log::getInstance()->setPattern("%D %T [%F:%L] %S: ");

	if (res == RPC_E_CHANGED_MODE) {

		logWARNING("COM RPC CHANGED MODE!");
		CoUninitialize();
		res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (res != S_OK) {
			logEMERGENCY("Impossible to initialize COM with correct thread model!");
			exit(-1);
		}
	}

	const char* libraries;
	const char* modules;
	cvGetModuleInfo(0, &libraries, &modules);

	logINFORMATIONAL("OpenCV Libraries: %s", libraries);
	logINFORMATIONAL("IPP Modules: %s", modules);

	Mocap mocap(argc, argv);

	return mocap.run();

}
