/*!
 * Class that implements a Blob.
 *
 * \name Blob
 * \author David Lunardi Flam
 * \version
 * \since 07/10/2008
 */
#include "Blob.h"

Blob::Blob() : _xMoment(0.0f), _yMoment(0.0f), _numPoints(0) {

}

Blob::~Blob() {

}

int Blob::getArea() {

	return _numPoints;
}

CvPoint2D32f Blob::getCentroid() {

	(_numPoints <= 0) ? _numPoints = 1 : _numPoints;

	CvPoint2D32f centroid;

	//TODO (+0.25 just for tests)
	centroid.x = _xMoment / (float)_numPoints + 0.25;
	centroid.y = _yMoment / (float)_numPoints;

	return centroid;
}
