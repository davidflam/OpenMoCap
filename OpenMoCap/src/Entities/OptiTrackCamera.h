#ifndef OPTITRACKCAMERA_H_
#define OPTITRACKCAMERA_H_

#include <iostream>
#include <optitrack.h>
#include <vector>

#include "AbstractCamera.h"
#include "../Utils/Utils.h"

using namespace std;

class OptiTrackCamera: public AbstractCamera {

public:

	/*!
	 * OptiTrackCamera constructor.
	 *
	 * @param id OptiTrack camera ID.
	 * @param width Width of the OptiTrack camera resolution.
	 * @param height Height of the OptiTrack camera resolution.
	 * @param frameRate OptiTrack camera frame rate.
	 * @param cameraCollectionIndex The OptiTrack camera index in the collection.
	 */
	OptiTrackCamera(int id, int width, int height, int frameRate, int cameraCollectionIndex);

	/*!
	 * OptiTrackCamera destructor.
	 */
	virtual ~OptiTrackCamera();

	/*!
	 * Initializes the OptiTrack cameras.
	 *
	 * @param _camerasRef A vector of OptiTrack cameras references.
	 * @param width Width of the OptiTrack cameras resolution.
	 * @param height Height of the OptiTrack cameras resolution.
	 * @param frameRate OptiTrack cameras frame rate.
	 */
	static void initializeOptiTrackCameras(vector<AbstractCamera*>* _camerasRef, int width, int height, int frameRate);

	/*!
	 * Releases the OptiTrack cameras.
	 */
	static void releaseOptiTrackCameras();

	bool setupDevice();

	void stopDevice();

	IplImage* getFrame();

	void setResolution(int width, int height);

	void setFrameRate(int frameRate);

private:

	//!A collection of OptiTrack cameras.
	static INPCameraCollection* CAMERA_COLLECTION;

	//! An OptiTrack camera.
	INPCamera *_camera;

	//! An OptiTrack camera frame.
	INPCameraFrame *_NPFrame;

	//! The index of the cameras collection.
	int _cameraCollectionIndex;
};

#endif /* OPTITRACKCAMERA_H_ */
