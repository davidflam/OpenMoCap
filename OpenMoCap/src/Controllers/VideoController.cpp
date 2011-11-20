/*!
 * Class that implements the thread responsible for the video controller.
 *
 * \name VideoController
 * \author David Lunardi Flam
 * \version
 * \since 10/14/2008
 */
#include "VideoController.h"

VideoController::VideoController(AbstractCamera* camera, AbstractCalibrator* calibrator, AbstractPOIFinder* POIFinder,
		AbstractTracker* tracker) :
	_videoStatus(VideoStatusEnum::PLAY_LIVE), _cameraRef(camera), _calibrator(calibrator), _POIFinder(POIFinder),
			_tracker(tracker) {

	poiDetectionEndEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	trackingEndEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
}

VideoController::~VideoController() {

	delete _calibrator;
	delete _POIFinder;
	delete _tracker;
}

void VideoController::run() {

	while (true) {

		if (_videoStatus == VideoStatusEnum::PLAY_LIVE) {

			IplImage* currentFrame = _cameraRef->getFrame();
			vector<POI> newPOIs = _POIFinder->getPOIsInImage(currentFrame);
			SetEvent(poiDetectionEndEvent);
			suspend();

			//--- Refreshes image already
			trackNewPOIs(newPOIs, currentFrame);
			SetEvent(trackingEndEvent);
			suspend();

		} else if (_videoStatus == VideoStatusEnum::RECORD) {

			IplImage* currentFrame = _cameraRef->getFrame();
			cvWriteFrame(_videoWriter, currentFrame);
			vector<POI> newPOIs;
			_POIsImageWidgetRef->refreshImage(currentFrame, newPOIs);

			SetEvent(poiDetectionEndEvent);
			suspend();

			SetEvent(trackingEndEvent);
			suspend();

		} else if (_videoStatus == VideoStatusEnum::CALIBRATE || _videoStatus == VideoStatusEnum::STOP) {

			SetEvent(poiDetectionEndEvent);
			suspend();
			SetEvent(trackingEndEvent);
			suspend();

		}

	}
}

void VideoController::trackNewPOIs(vector<POI> detectedPOIs, IplImage* currentFrame) {

	map<string, POI> currentInitializedPOIs = _cameraRef->getPOIs();

	int radius = 50;

	//--- Tracker refreshes map of current initialized POIs with newly detected ones.
	_tracker->refreshPOIsPosition(currentInitializedPOIs, detectedPOIs, _cameraRef->getWidth() - 1, _cameraRef->getHeight() - 1, radius);

	//--- We must update cameras POIs
	_cameraRef->setPOIs(currentInitializedPOIs);

	//--- Builds display list
	vector<POI> POIsDisplayList = detectedPOIs;

	//---Insert initialized POIs into POIs display list
	for (map<string, POI>::const_iterator it = currentInitializedPOIs.begin(); it != currentInitializedPOIs.end(); ++it) {
		POIsDisplayList.push_back(it->second);
	}

	//--- Updates image
	_POIsImageWidgetRef->refreshImage(currentFrame, POIsDisplayList);
}

void VideoController::stopVideoCapture() {

	//--- If recording, stops it!
	if (_videoStatus == VideoStatusEnum::RECORD) {
		logDEBUG("Stopped recording video from %s.", _cameraRef->getName().c_str());
		cvReleaseVideoWriter(&_videoWriter);
	}

	_videoStatus = VideoStatusEnum::STOP;
}

void VideoController::startVideoCapture() {

	_videoStatus = VideoStatusEnum::PLAY_LIVE;
}

void VideoController::startRecordingVideo(const char* videoFilePath) {

	_videoStatus = VideoStatusEnum::RECORD;

	logDEBUG("Started recording video from %s.", _cameraRef->getName().c_str());

	//--- video without compression, we don't want artifacts while testing algoritms
	_videoWriter = cvCreateVideoWriter(videoFilePath, CV_FOURCC_DEFAULT, _cameraRef->getFrameRate(), cvSize(_cameraRef->getWidth(), _cameraRef->getHeight()));
}


void VideoController::restartTracker(){

	//_tracker->restartTracker();

}

