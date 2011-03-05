/*!
 * Abstract class for trackers.
 *
 * \name AbstractTracker
 * \author David Lunardi Flam
 * \version
 * \since 02/18/2009
 */
#include "AbstractTracker.h"
#include "../Utils/Log.h"

AbstractTracker::AbstractTracker() {

}

AbstractTracker::~AbstractTracker() {

}

void AbstractTracker::test(AbstractPOIFinder* poiFinder, string filePath) {

	CvCapture* videoInputFile = cvCreateFileCapture(filePath.c_str());

	if (videoInputFile == NULL) {
		logEMERGENCY("file could not be found. <%s>", filePath.c_str());
		exit(-1);
	}

	IplImage* frame = cvQueryFrame(videoInputFile);
	int width = cvGetCaptureProperty(videoInputFile, CV_CAP_PROP_FRAME_WIDTH);
	int height = cvGetCaptureProperty(videoInputFile, CV_CAP_PROP_FRAME_HEIGHT);
	double frameRate = cvGetCaptureProperty(videoInputFile, CV_CAP_PROP_FPS);

	logDEBUG("Testing Tracker with Video <%s>", filePath.c_str());
	logDEBUG("%d x %d", width, height);
	logDEBUG("%d fps", frameRate);

	//--- Auto assign labels to detectedPOIs in first frame.
	vector<POI> POIs = poiFinder->getPOIsInImage(frame);
	map<string, POI> currentPOIs;

	for (unsigned int i = 0; i < POIs.size(); ++i) {

		QString semantic(Utils::toString(i).c_str());

		POIs[i].setSemantic(semantic);
		POIs[i].setPredictedPosition(POIs[i].getCoordinates2d());

		currentPOIs[semantic.toStdString()] = POIs[i];
	}

	int frameCount = 0;
	do {

		for (map<string, POI>::const_iterator it = currentPOIs.begin(); it != currentPOIs.end(); ++it) {
			POI poi = it->second;
			logDEBUG(poi.getInfo2d().toStdString().c_str());
		}

		vector<POI> detectedPOIs = poiFinder->getPOIsInImage(frame);
		int radius;
		refreshPOIsPosition(currentPOIs, detectedPOIs, width, height, radius);

		frameCount++;
	} while ((frame = cvQueryFrame(videoInputFile)) != NULL);

	logDEBUG("Processed %d frames", frameCount);
}



void restartTracker(){
	;
}

