#ifndef CAPTURECONTROLLER_H_
#define CAPTURECONTROLLER_H_

#include "VideoController.h"
#include "../Gui/Widgets/ImageWidget.h"
#include "../Entities/AbstractCamera.h"
#include "../Enums/CaptureStatusEnum.h"
#include "../Utils/Timer.h"
#include "../Utils/AbstractThread.h"
#include "../Mocap.h"
#include "../Reconstruction/AbstractReconstructor.h"
#include "../Output/TrcFileWriter.h"
#include "../Utils/Log.h"

//--- Forward Declaration
class Mocap;
class MainWindow;

//! Maximum number of video controllers.
#define MAX_VIDEO_CONTROLLERS 10

/*!
 * The completion routine. Will be executed when the timer is signaled.
 *
 * @param parameter The capture controller.
 * @param b The low time value of the time at which the timer was signaled.
 * @param c The high time value of the time at which the timer was signaled.
 */
void __stdcall timerEvent(void* parameter, DWORD b, DWORD c);

using namespace std;

class CaptureController: public AbstractThread {

public:

	/*!
	 * CaptureController constructor.
	 *
	 * @param mocap Mocap object.
	 */
	CaptureController(Mocap* mocap, MainWindow* mocapMainWindow);

	/*!
	 * CaptureController destructor.
	 */
	virtual ~CaptureController();

	/*!
	 * Gets the capture status.
	 *
	 * @return The capture status.
	 */
	CaptureStatusEnum getCaptureStatus() const {
		return _captureStatus;
	}

	/*!
	 * Sets the capture status.
	 *
	 * @param _captureStatus The capture status to be used.
	 */
	void setCaptureStatus(CaptureStatusEnum _captureStatus) {
		this->_captureStatus = _captureStatus;
	}

	void run();

	/*!
	 * Adds a new video controller to the capture process, if it is possible.
	 *
	 * @param videoController VideoController object to be added.
	 */
	void addVideoController(VideoController* videoController);

	/*!
	 * Refreshes the capture time.
	 */
	void refreshCaptureTime();

	friend void timerEvent(void *parameter, DWORD b, DWORD c);

	/*!
	 * Gets the capture time.
	 *
	 * @return The capture time.
	 */
	QTime getCaptureTime() const {
		return _captureTime;
	}

	void setVisualizationRef(ThreeDWidget* visualizationRef) {
		_visualizationRef = visualizationRef;
	}

	ThreeDWidget* getVisualizationRef() {
		return _visualizationRef;
	}

	Mocap* getMocap() {
		return _mocapRef;
	}

	VideoController* getVideoController(int cameraIndex) {
		return _videoControllersRef[cameraIndex];
	}

private:

	//! Capture time.
	QTime _captureTime;

	//! Capture status.
	CaptureStatusEnum _captureStatus;

	//! Mocap instance.
	Mocap* _mocapRef;

	//! Vector containing the VideoController instances.
	VideoController* _videoControllersRef[MAX_VIDEO_CONTROLLERS];

	//! Number of video controllers.
	int _numVideoControllers;

	//! Reconstruction algorithm
	AbstractReconstructor* _reconstructor;

	//! Output Writer algorithm
	AbstractOutputWriter* _outputWriter;

	//! ThreeDWidget Reference
	ThreeDWidget* _visualizationRef;

	//! ThreeDWidget Reference
	MainWindow* _mocapMainWindowRef;
};

#endif /* CAPTURECONTROLLER_H_ */
