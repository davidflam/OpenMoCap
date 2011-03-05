#ifndef ABSTRACTRECONSTRUCTOR_H_
#define ABSTRACTRECONSTRUCTOR_H_

#include <opencv2/opencv.hpp>
#include <map>
#include <vector>

#include "../Entities/POI.h"
#include "../Entities/AbstractCamera.h"

using namespace std;

class AbstractReconstructor {

public:
	/*!
	 * Using initialized POIs from two cameras reconstruct them, returning a vector containing all detected POIs
	 * with z coordinate
	 */
	virtual vector<POI> reconstructPOIs(AbstractCamera* camera1, AbstractCamera* camera2) = 0;

	/*!
	 * Computes point arrays from cameras. Image coordinates are left-handed, we must use the original coordinates complement
	 * to turn it to right-handed system
	 */
	static void computePointArrays(map<string, POI> firstCameraPOIs, map<string, POI> secondCameraPOIs, CvMat* points1,
			CvMat* points2, int width, int height);

	static void computePointArrays(vector< map<string, POI> >& CameraPOIs, vector<CvMat*>& points, int width, int height);


	/*!
	 * Triangulates two inhomogeneous points into a 3d homogeneous point (w = 1) with camera projection matrices
	 */
	static CvMat* compute3dHomogeneousCoordinates(CvMat* projectionMatrix1, CvMat* projectionMatrix2,
			CvPoint2D32f point1, CvPoint2D32f point2);

	/*!
	 * Triangulates two inhomogeneous points into a 3d homogeneous point (w = 1) with camera projection matrices
	 */
	static CvMat* compute3dHomogeneousCoordinates(vector<CvMat*>& projectionMatrix, vector<CvPoint2D32f>& point);
protected:

	AbstractReconstructor() {
	}

};

#endif /* ABSTRACTRECONSTRUCTOR_H_ */
