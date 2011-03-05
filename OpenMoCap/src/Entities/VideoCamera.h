#ifndef VIDEOCAMERA_H_
#define VIDEOCAMERA_H_

#include "AbstractCamera.h"

#include <sstream>
#include <videoInput.h>

class VideoCamera: public AbstractCamera {

public:

	/*!
	 * VideoCamera constructor.
	 *
	 * @param id video camera ID.
	 * @param width Width of the video camera resolution.
	 * @param height Height of the video camera resolution.
	 * @param frameRate video camera frame rate.
	 * @param videoLocation in case of start the camera with a video.
	 */
	VideoCamera(int id, int width, int height, int frameRate);
	VideoCamera(int id, int width, int height, int frameRate, QString videoLocation);

	/*!
	 * VideoCamera destructor.
	 */
	~VideoCamera();

	bool setupDevice();

	void stopDevice();

	IplImage* getFrame();

	void showSettingsWindow();

	void setResolution(int width, int height);

	void setFrameRate(int frameRate);

	void setSourceVideo(QString videoLocation);

	void resetVideo();

	void stopVideo();

	void playVideo();

private:

	//! Initiate a video capture
	CvCapture *_vidCapture;

	//! Returns the actual frame
	IplImage *_vidFrame;

	//! Got the total frame numbers of the video that is running on the camera
	long _vidFrameNumbers;

	//! Got the number of the actual frame of the video that is running on the camera
	long _vidActualFrameNumber;

	//! The relative position of the video
	float _vidActualPos;

	//! Determines if there is a video running on the camera or not
	bool _gotVideo;

	//! Determine if the video is playing or not
	bool _isPlaying;

};

#endif /* VIDEOCAMERA_H_ */
