#ifndef POI_H_
#define POI_H_

#include <opencv2/opencv.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <iostream>
#include <fstream>
#include <QPainter>
#include <QString>
#include <string>

using namespace std;

class POI {

public:

	/*!
	 * POI constructor with a CvPoint to initialize the 2D coordinates.
	 *
	 * @param coordinates2d 2D "POI" coordinates as a CvPoint.
	 */
	POI(CvPoint2D32f coordinates2d);

	/*!
	 * POI constructor with a CvPoint to initialize the 2D coordinates.
	 *
	 * @param coordinates2d 2D "POI" coordinates as a CvPoint.
	 * @param isCalibrationPoint indicates if it is a calibration point, different drawing.
	 */
	POI(CvPoint2D32f coordinates2d, bool isCalibrationPoint);

	/*!
	 * POI constructor with two integers to initialize the 2D coordinates.
	 *
	 * @param x "POI" coordinate x.
	 * @param y "POI" coordinate y.
	 */
	POI(float x = 0, float y = 0);

	/*!
	 * POI constructor with a CvPoint3D32f to initialize the 3D coordinates.
	 *
	 * @param coordinates3d 3D "POI" coordinates as a CvPoint3D32f.
	 */
	POI(CvPoint3D32f coordinates3d);

	/*!
	 * POI constructor with three integers to initialize the 3D coordinates.
	 *
	 * @param x "POI" coordinate x.
	 * @param y "POI" coordinate y.
	 * @param z "POI" coordinate z.
	 */
	POI(float x, float y, float z);

	/*!
	 * POI destructor.
	 */
	~POI();

	/*!
	 * Gets the 2D "POI" coordinates.
	 *
	 * @return The 2D "POI" coordinates.
	 */
	CvPoint2D32f getCoordinates2d() const {
		return _coordinates2d;
	}

	/*!
	 * Sets the 2D "POI" coordinates.
	 *
	 * @param _coordinates2d 2D coordinates do be used.
	 */
	void setCoordinates2d(CvPoint2D32f _coordinates2d) {
		this->_coordinates2d = _coordinates2d;
	}

	/*!
	 * Gets the 3D "POI" coordinates.
	 *
	 * @return The 3D "POI" coordinates.
	 */
	CvPoint3D32f getCoordinates3d() const {
		return _coordinates3d;
	}

	/*!
	 * Sets the 3D "POI" coordinates".
	 *
	 * @param _coordinates3d 3D coordinates to be used.
	 */
	void setCoordinates3d(CvPoint3D32f _coordinates3d) {
		this->_coordinates3d = _coordinates3d;
	}

	/*!
	 * Gets the 2D velocity vector of the "POI".
	 *
	 * @return The 2D velocity vector of the "POI".
	 */
	CvPoint2D32f getVelocityVector2d() const {
		return _velocityVector2d;
	}

	/*!
	 * Sets the 2D velocity vector of the "POI".
	 *
	 * @param _velocityVector2d 2D velocity vector to be used.
	 */
	void setVelocityVector2d(CvPoint2D32f _velocityVector2d) {
		this->_velocityVector2d = _velocityVector2d;
	}

	/*!
	 * Gets the "POI" semantic.
	 *
	 * @return The "POI" semantic.
	 */
	QString getSemantic() const {
		return _semantic;
	}

	/*!
	 * Sets the "POI" semantic.
	 *
	 * @param _semantic The semantic to be used.
	 */
	void setSemantic(QString _semantic) {
		this->_semantic = _semantic;
	}

	/*!
	 * Draws the circle that represents the "POI". If the "POI" already have semantic, draws a red circle, otherwise, a green circle.
	 *
	 * @param painter The object to draw the circle.
	 */
	void draw2d(QPainter* painter);

	/*!
	 * Verifies if the "POI" is inside a 2D selection area.
	 *
	 * @param pointX Coordinate x of the selection area.
	 * @param pointY Coordinate y of the selection area.
	 *
	 * @return If the "POI" is inside the selection area, this function returns "true", otherwise, "false".
	 */
	bool isPointInsideSelectionArea2d(int& pointX, int& pointY);

	/*!
	 *
	 * @return true if POI has an associated semantic else false.
	 */
	bool isInitialized();

	/*!
	 * Null semantic constant
	 * "NULL"
	 */
	static const string NULL_SEMANTIC_STRING;

	/*!
	 * Shows some debugging info
	 */
	QString getInfo3d() const;

	QString getInfo2d() const;

	CvPoint2D32f getPredictedPosition() const {
		return _predictedPosition;
	}

	void setPredictedPosition(CvPoint2D32f _predictedPosition) {
		this->_predictedPosition = _predictedPosition;
	}

	/*!
	 * Sets the "POI" Kalman tracker.
	 *
	 * @param k The Kalman tracker to be used.
	 */
	void setKalman(CvKalman* k) {
		this->_kalman = k;
	}

	/*!
	 * Sets the "POI" Condensation tracker.
	 *
	 * @param c The Condensation tracker to be used.
	 */
	void setCondensation(CvConDensation* c) {
		this->_condensation = c;
	}

	/*!
	 * Gets the "POI" Kalman tracker.
	 *
	 * @return The "POI" Kalman tracker.
	 */
	CvKalman* getKalman() const {
		return this->_kalman;
	}

	/*!
	 * Gets the "POI" Condensation tracker.
	 *
	 * @return The "POI" Condensation tracker.
	 */
	CvConDensation* getCondensation() const {
		return this->_condensation;
	}

	void setCalibrationPoint(bool isCalibrationPoint){
		_calibrationPoint = isCalibrationPoint;
	}

	bool getCalibrationPoint(){
		return _calibrationPoint;
	}

private:

	//! "POI" radius.
	static const int DRAW_RADIUS = 5;

	//! "POI" diameter.
	static const int DRAW_DIAMETER = DRAW_RADIUS * 2;

	//! 2D "POI" coordinates.
	CvPoint2D32f _coordinates2d;

	//! 2D velocity vector of the "POI".
	CvPoint2D32f _velocityVector2d;

	//! 3D "POI" coordinates.
	CvPoint3D32f _coordinates3d;

	//! "POI" semantic.
	QString _semantic;

	//! "Predicted Position"
	CvPoint2D32f _predictedPosition;

	//! POI Kalman tracker
	CvKalman* _kalman;

	//! POI Condensation tracker
	CvConDensation* _condensation;

	bool _calibrationPoint;

};

#endif /* POI_H_ */
