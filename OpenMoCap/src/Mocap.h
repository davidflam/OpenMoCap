#ifndef MOCAP_H_
#define MOCAP_H_

#include <stdio.h>

#include <csignal>
#include <fstream>
#include <map>
#include <QApplication>
#include <videoInput.h>

#include "Calibration/ZhangCalibrator.h"
#include "Entities/DirectShowCamera.h"
#include "Entities/OptiTrackCamera.h"
#include "Entities/DummyCamera.h"
#include "Entities/VideoCamera.h"
#include "Enums/CalibrationAlgorithmEnum.h"
#include "Enums/POIFinderAlgorithmEnum.h"
#include "Enums/ReconstructionAlgorithmEnum.h"
#include "Enums/TrackingAlgorithmEnum.h"
#include "Gui/MainWindow.h"
#include "Input/BvhReader.h"
#include "Reconstruction/BasicReconstructor.h"
#include "Tracking/AlphaBetaGammaFilter.h"
#include "Tracking/KalmanFilter.h"
#include "Tracking/Condensation.h"
#include "Tracking/BlobExtractor.h"
#include "Output/TrcFileWriter.h"
#include "Utils/Log.h"

//--- Forward Declaration
class MainWindow;

/*!
 * Main.
 *
 * @param argc Number of program arguments.
 * @param argv Vector that contains the program arguments.
 *
 * @return 0 if the program was finished OK, otherwise 1.
 */
int main(int argc, char* argv[]);

using namespace std;

class Mocap {

public:

	/*!
	 * Mocap constructor.
	 *
	 * @param argc Number of program arguments.
	 * @param argv Vector that contains the program arguments.
	 */
	Mocap(int argc, char* argv[]);

	/*!
	 * Mocap destructor.
	 */
	~Mocap();

	/*!
	 * Runs the GUI application.
	 *
	 * @return 0 if the GUI application was finished OK, otherwise 1.
	 */
	int run();

	/*!
	 * Sets the vector of cameras with the argument.
	 *
	 * @param cameras A vector of cameras.
	 */
	void setCameras(vector<AbstractCamera*> cameras) {
		this->_cameras = cameras;
	}

	/*!
	 * Gets the vector of cameras.
	 *
	 * @return The vector of cameras.
	 */
	vector<AbstractCamera*>* getCameras() {
		return &(_cameras);
	}

	/*!
	 * Gets the list of "POI" semantic types.
	 *
	 * @return The list of "POI" semantic types.
	 */
	QStringList* getPOISemanticTypes() {
		return _skeleton.getJointsNames();
	}

	/*!
	 * Gets the calibration algorithm used.
	 *
	 * @return The calibration algorithm used.
	 */
	AbstractCalibrator* getCalibrationAlgorithm();

	/*!
	 * Sets the enumeration of calibration algorithms with the argument.
	 *
	 * @param calibrationAlgorithmType An enumeration of calibration algorithms.
	 */
	void setCalibrationAlgorithmType(CalibrationAlgorithmEnum calibrationAlgorithmType) {
		_calibrationAlgorithmType = calibrationAlgorithmType;
	}

	/*!
	 * Gets the Point Of Interest("POI") finder algorithm used.
	 *
	 * @return The "POI" finder algorithm used.
	 */
	AbstractPOIFinder* getPOIFinderAlgorithm();

	/*!
	 * Sets the enumeration of finder algorithms with the argument.
	 *
	 * @param POIFinderAlgorithmType An enumeration of finder algorithms.
	 */
	void setPOIFinderAlgorithmType(POIFinderAlgorithmEnum POIFinderAlgorithmType) {
		_POIFinderAlgorithmType = POIFinderAlgorithmType;
	}

	/*!
	 * Get the tracking algorithm used.
	 *
	 * @return The tracking algorithm used.
	 */
	AbstractTracker* getTrackingAlgorithm();

	/*!
	 * Sets tracking algorithm type
	 */
	void setTrackingAlgorithmType(TrackingAlgorithmEnum trackingAlgorithmType) {
		_trackingAlgorithmType = trackingAlgorithmType;
	}

	/*!
	 * Get the reconstrution algorithm used.
	 *
	 * @return The reconstruction algorithm used.
	 */
	AbstractReconstructor* getReconstructionAlgorithm();

	/*!
	 * Sets reconstruction algorithm type
	 */
	void setReconstructionAlgorithmType(ReconstructionAlgorithmEnum reconstructionAlgorithmType) {
		_reconstructionAlgorithmType = reconstructionAlgorithmType;
	}

	/*!
	 * Get the output writer algorithm used.
	 * @return The output writer algorithm used.
	 */
	AbstractOutputWriter* getOutputWriterAlgorithm();

	/*!
	 * Gets the camera resolution width.
	 *
	 * @return The camera resolution width.
	 */
	int getCameraResolutionWidth() const {
		return _cameraResolutionWidth;
	}

	/*!
	 * Sets the camera resolution width.
	 *
	 * @param _cameraResolutionWidth Camera resolution width to be used.
	 */
	void setCameraResolutionWidth(int _cameraResolutionWidth) {
		this->_cameraResolutionWidth = _cameraResolutionWidth;
	}

	/*!
	 * Gets the camera resolution height.
	 *
	 * @return The camera resolution height.
	 */
	int getCameraResolutionHeight() const {
		return _cameraResolutionHeight;
	}

	/*!
	 * Sets the camera resolution height.
	 *
	 * @param _cameraResolutionHeight Camera resolution height to be used.
	 */
	void setCameraResolutionHeight(int _cameraResolutionHeight) {
		this->_cameraResolutionHeight = _cameraResolutionHeight;
	}

	/*!
	 * Gets the camera frame rate.
	 *
	 * @return The camera frame rate.
	 */
	int getCameraFrameRate() const {
		return _cameraFrameRate;
	}

	/*!
	 * Sets the camera frame rate.
	 *
	 * @param _cameraFrameRate Camera frame rate to be used.
	 */
	void setCameraFrameRate(int _cameraFrameRate) {
		this->_cameraFrameRate = _cameraFrameRate;
	}

	/*!
	 * Returns a reference to skeleton
	 */
	Skeleton& getSkeleton() {
		return _skeleton;
	}

private:

	//! Object that control the flow and main settings of the GUI application.
	QApplication* _mocapApp;

	//! Object of videoInput library.
	videoInput _videoInput;

	//! Vector of cameras.
	vector<AbstractCamera*> _cameras;

	//! Camera resolution width.
	int _cameraResolutionWidth;

	//! Camera resolution height.
	int _cameraResolutionHeight;

	//! Camera frame rate.
	int _cameraFrameRate;

	//! Number of video cameras that will be initialized
	int _videoCamerasNumber;

	//! Capture skeleton
	Skeleton _skeleton;

	/*!
	 * Determines the default values for the camera properties.
	 */
	void addDefaultCameraProperties();

	/*!
	 * Initializes the camera properties with those contained in the camera properties file if it exists,
	 * otherwise initializes it with the default values.
	 */
	void initializeCameraProperties();

	/*!
	 * Initializes the vector of active cameras.
	 */
	void initializeCameras();

	/*!
	 * Loads camera calibration data.
	 */
	void loadCalibrationData();

	/*!
	 * Releases the vector of active cameras.
	 */
	void releaseCameras();

	//! Enumeration of calibration algorithms.
	CalibrationAlgorithmEnum _calibrationAlgorithmType;

	//! Enumeration of "POI" finder algorithms.
	POIFinderAlgorithmEnum _POIFinderAlgorithmType;

	//! Enumeration of tracking algorithms
	TrackingAlgorithmEnum _trackingAlgorithmType;

	//! Enumeration of reconstruction algorithms
	ReconstructionAlgorithmEnum _reconstructionAlgorithmType;

	/*!
	 * Initializes skeleton based on a BVH file
	 */
	void initializeSkeleton();

	/*!
	 * Segmentation Fault Handler
	 */
	void signalHandler (int code);

};

#endif /* MOCAP_H_ */
