#ifndef BASICRECONSTRUCTOR_H_
#define BASICRECONSTRUCTOR_H_

#include "AbstractReconstructor.h"
#include "../Utils/Utils.h"
#include "../Utils/Timer.h"

#include <fstream>

#include "../Utils/Log.h"

using namespace std;

class BasicReconstructor: public AbstractReconstructor {

public:
	BasicReconstructor();

	virtual ~BasicReconstructor();

	vector<POI> reconstructPOIs(AbstractCamera* camera1, AbstractCamera* camera2);

	vector<POI> test();

private:

	void computeFundamentalMatrix(CvMat* fundamentalMatrix, CvMat* points1, CvMat* points2, int nPOIs);

	void computeCameraMatrices(CvMat* p1, CvMat* p2, CvMat* fundamentalMatrix);

	vector<POI> computeLinearEstimationOf3dCoordinates(CvMat* p1, CvMat* p2, CvMat* points1, CvMat* points2,
			int cameraWidth, int cameraHeight, map<string, POI> firstCameraPOIs);

	CvMat* computeContractionByEpsilonTensor(CvMat* mat, int rows, int columns);

	CvMat* computeNormalizedPoints(CvMat *points, int nPoints, CvMat* normalizationTransform);
};

#endif /* BASICRECONSTRUCTOR_H_ */
