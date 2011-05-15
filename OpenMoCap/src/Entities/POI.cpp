/*!
 * Point Of Interest class.
 *
 * \name POI
 * \author David Lunardi Flam
 * \version
 * \since 01/18/2009
 */
#include "POI.h"

const string POI::NULL_SEMANTIC_STRING("NULL");

POI::POI(CvPoint2D32f coordinates2d) :
	_coordinates2d(coordinates2d), _velocityVector2d(cvPoint2D32f(0, 0)), _semantic(NULL_SEMANTIC_STRING.c_str()),
			_predictedPosition(cvPoint2D32f(0.0f, 0.0f)), _kalman(NULL), _condensation(NULL), _calibrationPoint(false){
}

POI::POI(CvPoint2D32f coordinates2d, bool isCalibrationPoint) :
	_coordinates2d(coordinates2d), _velocityVector2d(cvPoint2D32f(0, 0)), _semantic(NULL_SEMANTIC_STRING.c_str()),
			_predictedPosition(cvPoint2D32f(0.0f, 0.0f)), _kalman(NULL), _condensation(NULL), _calibrationPoint(isCalibrationPoint) {
}

POI::POI(float x, float y) :
	_coordinates2d(cvPoint2D32f(x, y)), _velocityVector2d(cvPoint2D32f(0, 0)), _semantic(NULL_SEMANTIC_STRING.c_str()),
			_predictedPosition(cvPoint2D32f(0.0f, 0.0f)), _kalman(NULL), _condensation(NULL), _calibrationPoint(false) {

}

POI::POI(float x, float y, float z) :
	_coordinates3d(cvPoint3D32f(x, y, z)), _semantic(NULL_SEMANTIC_STRING.c_str()), _predictedPosition(cvPoint2D32f(
			0.0f, 0.0f)), _kalman(NULL), _condensation(NULL), _calibrationPoint(false) {

}

POI::POI(CvPoint3D32f coordinates3d) :
	_coordinates3d(coordinates3d), _semantic(NULL_SEMANTIC_STRING.c_str()),
			_predictedPosition(cvPoint2D32f(0.0f, 0.0f)), _kalman(NULL), _condensation(NULL), _calibrationPoint(false) {

}

/*!
 * POI destructor.
 */
POI::~POI() {

	/*if (this->getKalman() != NULL){

		CvKalman* k = this->getKalman();
		cvReleaseKalman(&k);
	}*/
	//cout << "Destruiu o POI: " << this->getSemantic().toStdString() << endl;
}

/*
 *
 */

void POI::draw2d(QPainter* painter) {

	//--- Draw POI Info
	if (!_calibrationPoint) {

		//--- No semantic red color, else green
		if (!isInitialized()) {
			painter->setPen(QColor(200, 0, 0));
			painter->setBrush(QColor(255, 0, 0, 50));
		} else {
			painter->setPen(QColor(0, 200, 0));
			painter->setBrush(QColor(0, 255, 0, 50));
		}

		QPoint centerPoint(_coordinates2d.x, _coordinates2d.y);
		painter->drawEllipse(centerPoint, DRAW_RADIUS, DRAW_RADIUS);
		painter->drawPoint(centerPoint);

		QString info = QString("%3 - <%1,%2>").arg(_coordinates2d.x).arg(_coordinates2d.y).arg(_semantic);

		painter->setPen(QColor(0, 0, 255));
		painter->drawText(_coordinates2d.x - DRAW_RADIUS, _coordinates2d.y - DRAW_RADIUS, info);

	} else {

		QPen pen(QBrush(QColor(0, 255, 0)), 2.0f);
		painter->setPen(pen);
		QLine horizontalLine(_coordinates2d.x - DRAW_RADIUS, _coordinates2d.y, _coordinates2d.x + DRAW_RADIUS, _coordinates2d.y);
		QLine verticalLine(_coordinates2d.x, _coordinates2d.y - DRAW_RADIUS, _coordinates2d.x, _coordinates2d.y + DRAW_RADIUS);
		painter->drawLine(horizontalLine);
		painter->drawLine(verticalLine);

	}
}

bool POI::isPointInsideSelectionArea2d(int& pointX, int& pointY) {

	double x = abs(_coordinates2d.x - pointX);
	double y = abs(_coordinates2d.y - pointY);

	double distance = sqrt((x * x) + (y * y));

	return (distance > DRAW_RADIUS) ? false : true;
}

bool POI::isInitialized() {

	if (_semantic.isNull() || _semantic.toStdString() == NULL_SEMANTIC_STRING) {
		return false;
	}
	return true;
}

QString POI::getInfo3d() const {

	QString info = QString("%1 - <%2, %3, %4>").arg(_semantic).arg(_coordinates3d.x).arg(_coordinates3d.y).arg(
			_coordinates3d.z);
	return info;

}

QString POI::getInfo2d() const {
	QString info = QString("<%3> - %1\t%2").arg(_coordinates2d.x).arg(_coordinates2d.y).arg(_semantic);
	return info;
}
