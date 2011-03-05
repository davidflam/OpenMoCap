#ifndef ABSTRACTCAMERA_H_
#define ABSTRACTCAMERA_H_

#include "POI.h"
#include "../Tracking/KalmanFilter.h"
#include "../Utils/Image.h"
#include "../Utils/Utils.h"

#include <map>
#include <string>

using namespace std;

/*!
 * This enum determines the kind of cameras on the system and give a number for them
 */
enum EnumCameraType { VIDEO, OPTITRACK, DIRECT, DUMMY };

class AbstractCamera {

public:

	/*!
	 * AbstractCamera virtual destructor.
	 */
	virtual ~AbstractCamera();

	/*!
	 * Gets the intrinsic camera parameters.
	 *
	 * @return The intrinsic camera parameters.
	 */
	CvMat *getIntrinsicParams() const {

		return _intrinsicParams;
	}

	/*!
	 * Sets the intrinsic camera parameters.
	 *
	 * @param _intrinsicParams The intrinsic camera parameters.
	 */
	void setIntrinsicParams(CvMat *_intrinsicParams) {

		this->_intrinsicParams = _intrinsicParams;
	}

	/*!
	 * Gets the distortion coefficients.
	 *
	 * @return The distortion coefficients.
	 */
	CvMat *getDistortionCoefficients() const {

		return _distortionCoefficients;
	}

	/*!
	 * Sets the distortion coefficients.
	 *
	 * @param _distortionCoefficients The distortion coefficients.
	 */
	void setDistortionCoefficients(CvMat *_distortionCoefficients) {

		this->_distortionCoefficients = _distortionCoefficients;
	}

	/*!
	 * Gets the calibration status.
	 *
	 * @return The calibration status.
	 */
	bool getCalibrated() const {

		return _calibrated;
	}

	/*!
	 * Sets the calibration status.
	 *
	 * @param _calibrated The calibration status.
	 */
	void setCalibrated(bool _calibrated) {

		this->_calibrated = _calibrated;
	}

	/*!
	 * Gets the camera name.
	 *
	 * @return The camera name.
	 */
	string getName() const {

		return _name;
	}

	/*!
	 * Sets the camera name.
	 *
	 * @param _name The camera name.
	 */
	void setName(string _name) {

		this->_name = _name;
	}

	/*!
	 * Gets the camera ID.
	 *
	 * @return The camera ID.
	 */
	int getId() const {

		return _id;
	}

	/*!
	 * Sets the camera ID.
	 *
	 * @param _id The camera ID.
	 */
	void setId(int _id) {

		this->_id = _id;
	}

	/*!
	 * Gets the width of camera resolution.
	 *
	 * @return The width of camera resolution.
	 */
	int getWidth() const {

		return _width;
	}

	/*!
	 * Sets the width of camera resolution.
	 *
	 * @param _width The width of camera resolution.
	 */
	void setWidth(int _width) {

		this->_width = _width;
	}

	/*!
	 * Gets the height of camera resolution.
	 *
	 * @return The height of camera resolution.
	 */
	int getHeight() const {

		return _height;
	}

	/*!
	 * Sets the height of camera resolution.
	 *
	 * @param _height The height of camera resolution.
	 */
	void setHeight(int _height) {

		this->_height = _height;
	}

	/*!
	 * Gets the camera frame rate.
	 *
	 * @return The camera frame rate.
	 */
	int getFrameRate() const {

		return _frameRate;
	}

	/*!
	 * Gets the camera POIs.
	 *
	 * @return The camera POIs.
	 */
	map<string, POI> getPOIs() const {

		return _POIs;
	}

	/*!
	 * Sets the camera POIs.
	 *
	 * @param _POIs The camera POIs.
	 */
	void setPOIs(map<string, POI> _POIs) {

		this->_POIs = _POIs;
	}
	/*!
	 * Sets a camera frame.
	 *
	 * @param _frame The frame to be used.
	 */
	void setFrame(Image *_frame) {
		this->_frame = _frame;
	}
	/*!
	 * Returns camera translation vector
	 */
	CvPoint3D32f getTranslation() {
		return _translation;
	}
	/*!
	 * Sets camera translation vector
	 */
	void setTranslation(CvPoint3D32f translation) {
		_translation = translation;
	}
	/*!
	 * Returns camera type
	 */
	int getCameraType() {
		return _cameraType;
	}
	/*!
	 * Sets camera type
	 */
	void setCameraType(int type) {
		_cameraType = type;
	}
	/*!
	 * Returns camera rotation vector
	 */
	CvPoint3D32f getRotation() {
		return _rotation;
	}
	/*!
	 * Sets camera rotation vector
	 */
	void setRotation(CvPoint3D32f rotation) {
		_rotation = rotation;
	}

	/*!
	 * Resets the camera parameters.
	 */
	void resetParameters();

	/*!
	 * Corrects the image in the argument to compensate the camera distortion.
	 *
	 * @param distortedImagePath The path of distorted image.
	 *
	 * @return A vector with the distorted image and the corrected image.
	 */
	Image* previewUndistort(string distortedImagePath);

	/*!
	 * Inserts a new Point of Interest with associated semantic into camera hashmap.
	 *
	 * @param POItoInsert
	 */
	void insertPOI(POI& POItoInsert);

	/*!
	 * Removes POI from POIs map
	 * @param semantic of the POI to be removed
	 */
	void removePOI(string semantic);

	/*!
	 * Shows the camera settings.
	 */
	//TODO Implement a settings window for cameras using setResolution and setFrameRate
	void showSettingsWindow() {}

	/*!
	 * Gets information about camera.
	 *
	 * @return Information about camera.
	 */
	QString getInfo();
	/*!
	 * Gets information about camera distortion coefficients.
	 *
	 * @return Information about camera distortion coefficients.
	 */
	QString getDistorcionCoefficientsInfo();

	/*!
	 * Gets information about camera intrinsic parameters.
	 *
	 * @return Information about camera intrinsic parameters.
	 */
	QString getIntrinsicParamsInfo();

	/*!
	 * Gets information about camera extrinsic parameters.
	 *
	 * @return Information about camera extrinsic parameters.
	 */
	QString getExtrinsicParamsInfo();

	/*!
	 * Gets information about current camera POIs
	 * @return Information about current camera POIs
	 */
	QString getCurrent2dPOIsInfo();

	/*!
	 * Computes camera rotation + translation matrix
	 * @param a rotation around x axis
	 * @param b rotation around y axis
	 * @param g rotation around z axis
	 * @param tX translation on x axis
	 * @param tY translation on y axis
	 * @param tZ translation on z axis
	 * @return camera's displacement matrix
	 */
	static CvMat* computeRtMatrix(double a, double b, double g, double tX, double tY, double tZ);

	/*!
	 * Computes camera intrinsic parameter matrix
	 * @param focalLength focal length
	 * @param pX principal point x coordinate value
	 * @param pY principal point y coordinate value
	 * @return
	 */
	static CvMat* computeKMatrix(double focalLength, float pX, float py);

	/*!
	 * Grabs a frame.
	 *
	 * @return A camera frame.
	 */
	virtual IplImage* getFrame() = 0;

	/*!
	 * Sets the camera resolution.
	 *
	 * @param width Width of the camera resolution.
	 * @param height Height of the camera resolution.
	 */
	virtual void setResolution(int width, int height) = 0;

	/*!
	 * Sets the camera frame rate.
	 *
	 * @param frameRate The camera frame rate.
	 */
	virtual void setFrameRate(int frameRate) = 0;

protected:

	/*!
	 * AbstractCamera constructor.
	 *
	 * @param id Camera ID.
	 * @param width Camera resolution width.
	 * @param height Camera resolution height.
	 * @param frameRate Camera frame rate.
	 */
	AbstractCamera(int id, int width, int height, int frameRate);

	/*!
	 * AbstractCamera constructor used to differentiate between the type of the cameras.
	 *
	 * @param id Camera ID.
	 * @param width Camera resolution width.
	 * @param height Camera resolution height.
	 * @param frameRate Camera frame rate.
	 * @param cameraType Camera Type
	 */
	AbstractCamera(int id, int width, int height, int frameRate, int type);

	/*!
	 * Does any necessary setup prior to capture.
	 *
	 * @return If the setup occurred OK, this function returns "true", otherwise "false".
	 */
	virtual bool setupDevice() = 0;

	/*!
	 * Shuts the device down.
	 */
	virtual void stopDevice() = 0;

	//! Camera name.
	string _name;

	//! A camera frame.
	Image *_frame;

	//! Camera ID.
	int _id;

	//! Width of camera resolution.
	int _width;

	//! Height of camera resolution.
	int _height;

	//! Camera frame rate.
	int _frameRate;

private:

	//! Camera type: 0 videoCamera; > 0 others...
	int _cameraType;

	//! Camera intrinsic parameters.
	CvMat *_intrinsicParams;

	//! Camera extrinsic parameter - translation vector.
	CvPoint3D32f _translation;

	//! Camera extrinsic parameter - rotation vector.
	CvPoint3D32f _rotation;

	//! Camera distortion coefficients.
	CvMat *_distortionCoefficients;

	//! Points of Interest in a frame.
	map<string, POI> _POIs;

	//! Calibration status.
	bool _calibrated;

};

#endif /* ABSTRACTCAMERA_H_ */
