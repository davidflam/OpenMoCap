/*!
 * Class that implements the threads responsible for the capture controller.
 *
 * \name CaptureController
 * \author David Lunardi Flam
 * \version
 * \since 01/29/2009
 */
#include "CaptureController.h"

void __stdcall timerEvent(void* parameter, DWORD b, DWORD c) {

	CaptureController* captureController = (CaptureController*) parameter;

	float poiDetectionTime = 0.0;
	Timer poiDetectionTimer;

	float trackingTime = 0.0;
	Timer trackingTimer;

	float triangulationTime = 0.0;
	Timer triangulationTimer;

	float outputTime = 0.0;
	Timer outputTimer;

	if (captureController->_captureStatus == CaptureStatusEnum::STOP) {

	} else {

		HANDLE events[captureController->_numVideoControllers];
		int maxWaitTimeInMSec = 1000 / captureController->_mocapRef->getCameraFrameRate();

		//--- POI Detection
		poiDetectionTimer.start();
		for (int i = 0; i < captureController->_numVideoControllers; i++) {
			captureController->_videoControllersRef[i]->resume();
			events[i] = captureController->_videoControllersRef[i]->poiDetectionEndEvent;
		}
		WaitForMultipleObjects(captureController->_numVideoControllers, events, TRUE, maxWaitTimeInMSec);
		poiDetectionTime = poiDetectionTimer.stop();

		//--- Tracking
		trackingTimer.start();
		for (int i = 0; i < captureController->_numVideoControllers; i++) {
			captureController->_videoControllersRef[i]->resume();
			events[i] = captureController->_videoControllersRef[i]->trackingEndEvent;
		}
		WaitForMultipleObjects(captureController->_numVideoControllers, events, TRUE, maxWaitTimeInMSec);
		trackingTime = trackingTimer.stop();

		//---If recording, must use Triangulation algorithm and save info to a file
		if (captureController->_captureStatus == CaptureStatusEnum::RECORDING) {
			captureController->refreshCaptureTime();

			//We need at least 2 cameras to reconstruct
			if (captureController->_numVideoControllers >= 2) {

				//--- Triangulation
				triangulationTimer.start();
				vector<POI> tridimensionalPOIs = captureController->_reconstructor->reconstructPOIs(
						captureController->_videoControllersRef[0]->getCamera(),
						captureController->_videoControllersRef[1]->getCamera());
				triangulationTime = triangulationTimer.stop();

				//--- Write data to some output format
				outputTimer.start();
				captureController->_outputWriter->addMoCapData(tridimensionalPOIs);
				outputTime = outputTimer.stop();

				//--- Update visualization
				captureController->_visualizationRef->setPointCloud(tridimensionalPOIs);
				captureController->_visualizationRef->update();

				//--- Update POIs Details
				captureController->_mocapMainWindowRef->updatePOIsInformationToolBar(tridimensionalPOIs);

			}

		} else {
			captureController->_captureTime.setHMS(0, 0, 0, 0);
		}

		// TIME COUNT
		//--- Faster than cout in this formatting context
		//printf("%f\t%f\t%f\t%f\n", poiDetectionTime, trackingTime, triangulationTime, outputTime);
		//fflush(stdout);

	}
}

CaptureController::CaptureController(Mocap* mocap, MainWindow* mocapMainWindow) :
	_captureStatus(CaptureStatusEnum::PLAYING), _mocapRef(mocap), _numVideoControllers(0), _reconstructor(
			mocap->getReconstructionAlgorithm()), _outputWriter(mocap->getOutputWriterAlgorithm()), _visualizationRef(
			NULL), _mocapMainWindowRef(mocapMainWindow) {
}

CaptureController::~CaptureController() {

	delete _reconstructor;
	delete _outputWriter;

}

void CaptureController::addVideoController(VideoController* videoController) {

	if (_numVideoControllers >= MAX_VIDEO_CONTROLLERS) {
		logERROR("I can't manage any more cameras!");
	} else {
		_videoControllersRef[_numVideoControllers] = videoController;
		_numVideoControllers++;
	}
}

void CaptureController::refreshCaptureTime() {

	int timeToAddInMSec = 1000 / _mocapRef->getCameraFrameRate();
	_captureTime = _captureTime.addMSecs(timeToAddInMSec);
}

void CaptureController::run() {

	const long long MILISECONDS = 10000LL;
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	//--- Start timer at 0 ms (unit is 100 nanoseconds)
	liDueTime.QuadPart = -1000 * MILISECONDS;

	//--- Create a waitable timer.
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);

	//--- Set timer using frameRate
	SetWaitableTimer(hTimer, &liDueTime, (1000 / _mocapRef->getCameraFrameRate()), timerEvent, this, FALSE);

	//---Zeros Capture Time
	_captureTime.setHMS(0, 0, 0, 0);

	while (true) {
		SleepEx(INFINITE, TRUE);
	}
}
