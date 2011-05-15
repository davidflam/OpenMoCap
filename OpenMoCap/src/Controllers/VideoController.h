#ifndef VIDEOCONTROLLER_H_
#define VIDEOCONTROLLER_H_

#include <iostream>
#include <QObject>

#include "../Calibration/AbstractCalibrator.h"
#include "../Entities/AbstractCamera.h"
#include "../Enums/VideoStatusEnum.h"
#include "../Tracking/AbstractPOIFinder.h"
#include "../Tracking/AbstractTracker.h"
#include "../Utils/AbstractThread.h"
#include "../Gui/Widgets/POIsImageWidget.h"

using namespace std;

class VideoController: public AbstractThread {

public:

	/*!
	 * VideoController constructor
	 * @param camera
	 * @param calibrator
	 * @param POIFinder
	 * @param tracker
	 * @return
	 */
	VideoController(AbstractCamera* camera = NULL, AbstractCalibrator* calibrator = NULL, AbstractPOIFinder* POIFinder =
			NULL, AbstractTracker* tracker = NULL);

	/*!
	 * VideoController destructor.
	 */
	~VideoController();

	void setVideoStatus(VideoStatusEnum videoStatus) {
		_videoStatus = videoStatus;
	}

	const VideoStatusEnum getVideoStatus() {
		return _videoStatus;
	}

	POIsImageWidget* getPOIsImageWidgetRef() const {
		return _POIsImageWidgetRef;
	}

	void setPOIsImageWidget(POIsImageWidget *POIsImageWidget) {
		this->_POIsImageWidgetRef = POIsImageWidget;
	}

	AbstractCamera *getCamera() const {
		return _cameraRef;
	}

	AbstractCalibrator *getCalibrator() const {
		return _calibrator;
	}

	AbstractPOIFinder *getPOIFinder() const {
		return _POIFinder;
	}

	/*!
	 * Stops the video capture.
	 */
	void stopVideoCapture();

	/*!
	 * Starts the video capture.
	 */
	void startVideoCapture();

	/*!
	 * Records video
	 */
	void startRecordingVideo(const char* videoFilePath);

	void run();

	HANDLE poiDetectionEndEvent;
	HANDLE trackingEndEvent;

	/*!
	 * Refresh the tracker in events as the load of a new video
	 */
	void restartTracker();

private:

	POIsImageWidget* _POIsImageWidgetRef;
	VideoStatusEnum _videoStatus;
	QTimer* _frameRateTimer;

	//!Camera Reference
	AbstractCamera* _cameraRef;

	//! Calibration Algorithm
	AbstractCalibrator* _calibrator;

	//! POI Algorithm
	AbstractPOIFinder* _POIFinder;

	//! Tracking
	AbstractTracker* _tracker;

	//! Write videos
	CvVideoWriter *_videoWriter;

	//! Track new POIs
	void trackNewPOIs(vector<POI> detectedPOIs, IplImage* currentFrame);
};

#endif /* VIDEOCONTROLLER_H_ */
