#ifndef DUMMYCAMERA_H_
#define DUMMYCAMERA_H_

#include "AbstractCamera.h"

#include <videoInput.h>

class DummyCamera: public AbstractCamera {

public:

	/*!
	 * DummyCamera constructor.
	 *
	 * @param id DirectShow camera ID.
	 * @param width Width of the DirectShow camera resolution.
	 * @param height Height of the DirectShow camera resolution.
	 * @param frameRate DirectShow camera frame rate.
	 * @param vInput Object of the videoInput library.
	 */
	DummyCamera(int id, int width, int height, int frameRate);

	/*!
	 * DummyCamera destructor.
	 */
	~DummyCamera();

	bool setupDevice();

	void stopDevice();

	IplImage* getFrame();

	void showSettingsWindow();

	void setResolution(int width, int height);

	void setFrameRate(int frameRate);


};

#endif /* DIRECTSHOWCAMERA_H_ */
