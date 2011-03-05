#ifndef BLOB_H_
#define BLOB_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;

class Blob {

private:

	//! The x moment of the blob.
	float _xMoment;

	//! The y moment of the blob.
	float _yMoment;

	//! The number of blob points.
	int _numPoints;

public:

	/*!
	 * Blob constructor.
	 */
	Blob();

	/*!
	 * Blob destructor.
	 */
	~Blob();

	/*!
	 * Gets the blob area.
	 *
	 * @return The blob area in number of points.
	 */
	int getArea();

	/*!
	 * Gets the blob centroid.
	 *
	 * @return The blob centroid as a CvPoint.
	 */
	CvPoint2D32f getCentroid();

	/*!
	 * Adds a point to the blob area. (Inline function)
	 *
	 * @param x The x moment of the point.
	 * @param y The y moment of the point.
	 */
	void addPoint(int& x, int& y) {

		_xMoment += x;
		_yMoment += y;
		_numPoints++;
	}
};

#endif /* BLOB_H_ */
