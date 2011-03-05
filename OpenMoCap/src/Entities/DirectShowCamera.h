#ifndef DIRECTSHOWCAMERA_H_
#define DIRECTSHOWCAMERA_H_

#include "AbstractCamera.h"

#include <videoInput.h>

class DirectShowCamera: public AbstractCamera {

public:

	/*!
	 * DirectShowCamera constructor.
	 *
	 * @param id DirectShow camera ID.
	 * @param width Width of the DirectShow camera resolution.
	 * @param height Height of the DirectShow camera resolution.
	 * @param frameRate DirectShow camera frame rate.
	 * @param vInput Object of the videoInput library.
	 */
	DirectShowCamera(int id, int width, int height, int frameRate, videoInput* vInput);

	/*!
	 * DirectShowCamera destructor.
	 */
	~DirectShowCamera();

	bool setupDevice();

	void stopDevice();

	IplImage* getFrame();

	void showSettingsWindow();

	void setResolution(int width, int height);

	void setFrameRate(int frameRate);

private:

	//! Object of videoInput library.
	videoInput* _videoInput;
};

#endif /* DIRECTSHOWCAMERA_H_ */
