/*!
 * Class that implements a basic two view reconstruction algorithm
 *
 * \name BasicReconstructor
 * \author David Lunardi Flam
 * \version
 * \since 03/14/2009
 */

#include "BasicReconstructor.h"

BasicReconstructor::BasicReconstructor() {

}

BasicReconstructor::~BasicReconstructor() {

}

vector<POI> BasicReconstructor::reconstructPOIs(AbstractCamera* camera1, AbstractCamera* camera2) {

	map<string, POI> firstCameraPOIs = camera1->getPOIs();
	map<string, POI> secondCameraPOIs = camera2->getPOIs();

	int nPOIsFirstCamera = firstCameraPOIs.size();
	int nPOIsSecondCamera = secondCameraPOIs.size();
	vector<POI> POIs3d;

	//--- All points must have a correspondent.
	if (nPOIsFirstCamera == nPOIsSecondCamera) {

		CvMat* points1 = cvCreateMat(1, nPOIsFirstCamera, CV_32FC2);
		CvMat* points2 = cvCreateMat(1, nPOIsFirstCamera, CV_32FC2);
		AbstractReconstructor::computePointArrays(firstCameraPOIs, secondCameraPOIs, points1, points2, camera1->getWidth(), camera2->getHeight());

		CvMat* Q = camera1->getDisparityToDepth();

		int i = 0;
		for (map<string, POI>::const_iterator it = firstCameraPOIs.begin(); it != firstCameraPOIs.end(); ++it) {

			POI tridimensionalPOI;
			tridimensionalPOI.setSemantic(QString(it->first.c_str()));

			CvPoint2D32f pointRightImage = cvPoint2D32f(points1->data.fl[i * 2], points1->data.fl[i * 2 + 1]);
			CvPoint2D32f pointLeftImage = cvPoint2D32f(points2->data.fl[i * 2], points2->data.fl[i * 2 + 1]);

			float d = pointRightImage.x - pointLeftImage.x;
			float x = pointLeftImage.x * cvmGet(Q, 0, 0) + cvmGet(Q, 0, 3);
			float y = pointLeftImage.y * cvmGet(Q, 1, 1) + cvmGet(Q, 1, 3);
			float z = cvmGet(Q, 2, 3);
			float w = d * cvmGet(Q, 3, 2) + cvmGet(Q, 3, 3);

			tridimensionalPOI.setCoordinates3d( cvPoint3D32f( x/w, y/w, z/w ) );
			POIs3d.push_back(tridimensionalPOI);
			i++;

		}

		//--- Clear heap
		cvReleaseMat(&points1);
		cvReleaseMat(&points2);

	} else {
		logERROR("Number of points between cameras do not match!!!");
	}

	return POIs3d;
}

vector<POI> BasicReconstructor::test() {

	ifstream pointsFile;
	pointsFile.open("Resources/reconstruction/points.txt");

	map<string, POI> pois1;
	map<string, POI> pois2;
	int nPoints = 0;

	if (pointsFile.is_open()) {

		string token;
		pointsFile >> token;
		nPoints = atoi(token.c_str());

		int i = 0;
		while (pointsFile >> token) {

			int x = atoi(token.c_str());

			pointsFile >> token;
			int y = atoi(token.c_str());

			POI poi(x, y);
			pointsFile >> token;
			poi.setSemantic(QString(token.c_str()));

			if (i < nPoints) {
				pois1[token] = poi;
			} else {
				pois2[token] = poi;
			}

			i++;
		}
	} else {
		logERROR("Points file not found!");
	}

	Timer t;
	t.start();

	CvMat* points1 = cvCreateMat(1, nPoints, CV_32FC2);
	CvMat* points2 = cvCreateMat(1, nPoints, CV_32FC2);
	computePointArrays(pois1, pois2, points1, points2, 640, 480);

	CvMat* fundamentalMatrix = cvCreateMat(3, 3, CV_32FC1);
	computeFundamentalMatrix(fundamentalMatrix, points1, points2, nPoints);

	CvMat* p1 = cvCreateMat(3, 4, CV_32FC1);
	CvMat* p2 = cvCreateMat(3, 4, CV_32FC1);
	computeCameraMatrices(p1, p2, fundamentalMatrix);

	vector<POI> POIs3d = computeLinearEstimationOf3dCoordinates(p1, p2, points1, points2, 640, 480, pois1);

	//--- Clear heap
	cvReleaseMat(&points1);
	cvReleaseMat(&points2);
	cvReleaseMat(&fundamentalMatrix);
	cvReleaseMat(&p1);
	cvReleaseMat(&p2);

	logINFORMATIONAL("Done processing in %fs",t.stop());

	return POIs3d;

}

void BasicReconstructor::computeFundamentalMatrix(CvMat* fundamentalMatrix, CvMat* p1, CvMat* p2, int nPOIs) {

	//--- OpenCV Approach
	//	CvMat* status = cvCreateMat(1, nPOIs, CV_8UC1);
	//	int fmCount = cvFindFundamentalMat(points1, points2, fundamentalMatrix, CV_FM_8POINT);
	//
	//	if (fmCount != 1) {
	//		cout << "[ERROR] Could not find fundamental matrix or found more than one " << endl;
	//	} else {
	//		Utils::cvPrintMat(fundamentalMatrix, 3, 3);
	//	}
	//
	//	cvReleaseMat(&status);

	//--- Based on Andrew Zisserman Matlab code


	CvMat *normalizationTransform1 = cvCreateMat(3, 3, CV_32FC1);
	CvMat *normalizedPoints1 = computeNormalizedPoints(p1, nPOIs, normalizationTransform1);

	CvMat *normalizationTransform2 = cvCreateMat(3, 3, CV_32FC1);
	CvMat *normalizedPoints2 = computeNormalizedPoints(p2, nPOIs, normalizationTransform2);

	// Build the constraint matrix
	CvMat* constraintMatrix = cvCreateMat(nPOIs, 9, CV_32FC1);
	for (int i = 0; i < nPOIs; i++) {
		for (int j = 0; j < 9; j++) {

			switch (j) {

			case 0:
				//--- p2.x * p1.x
				cvmSet(constraintMatrix, i, j, normalizedPoints2->data.fl[i * 2] * normalizedPoints1->data.fl[i * 2]);
				break;
			case 1:
				//--- p2.x * p1.y
				cvmSet(constraintMatrix, i, j, normalizedPoints2->data.fl[i * 2]
						* normalizedPoints1->data.fl[i * 2 + 1]);
				break;
			case 2:
				//--- p2.x
				cvmSet(constraintMatrix, i, j, normalizedPoints2->data.fl[i * 2]);
				break;
			case 3:
				//--- p2.y * p1.x
				cvmSet(constraintMatrix, i, j, normalizedPoints2->data.fl[i * 2 + 1]
						* normalizedPoints1->data.fl[i * 2]);
				break;
			case 4:
				//--- p2.y * p1.y
				cvmSet(constraintMatrix, i, j, normalizedPoints2->data.fl[i * 2 + 1] * normalizedPoints1->data.fl[i * 2
						+ 1]);
				break;
			case 5:
				//--- p2.y
				cvmSet(constraintMatrix, i, j, normalizedPoints2->data.fl[i * 2 + 1]);
				break;
			case 6:
				//--- p1.x
				cvmSet(constraintMatrix, i, j, normalizedPoints1->data.fl[i * 2]);
				break;
			case 7:
				//--- p1.y
				cvmSet(constraintMatrix, i, j, normalizedPoints1->data.fl[i * 2 + 1]);
				break;
			default:
				cvmSet(constraintMatrix, i, j, 1);
			}

		}
	}

	cvReleaseMat(&normalizedPoints1);
	cvReleaseMat(&normalizedPoints2);

	CvMat* u = cvCreateMat(nPOIs, 9, CV_32FC1);
	CvMat* d = cvCreateMat(nPOIs, 9, CV_32FC1);
	CvMat* v = cvCreateMat(nPOIs, 9, CV_32FC1);
	cvSVD(constraintMatrix, d, u, v);

	cvReleaseMat(&constraintMatrix);
	cvReleaseMat(&d);
	cvReleaseMat(&u);

	//--- Extract fundamental matrix from the column of V corresponding to smallest singular value.
	cvmSet(fundamentalMatrix, 0, 0, cvmGet(v, 0, 8));
	cvmSet(fundamentalMatrix, 0, 1, cvmGet(v, 1, 8));
	cvmSet(fundamentalMatrix, 0, 2, cvmGet(v, 2, 8));
	cvmSet(fundamentalMatrix, 1, 0, cvmGet(v, 3, 8));
	cvmSet(fundamentalMatrix, 1, 1, cvmGet(v, 4, 8));
	cvmSet(fundamentalMatrix, 1, 2, cvmGet(v, 5, 8));
	cvmSet(fundamentalMatrix, 2, 0, cvmGet(v, 6, 8));
	cvmSet(fundamentalMatrix, 2, 1, cvmGet(v, 7, 8));
	cvmSet(fundamentalMatrix, 2, 2, cvmGet(v, 8, 8));

	cvReleaseMat(&v);

	//--- Enforce constraint that fundamental matrix has rank 2 by performing a svd and then reconstructing with the two largest singular values.
	CvMat* uF = cvCreateMat(3, 3, CV_32FC1);
	CvMat* dF = cvCreateMat(3, 3, CV_32FC1);
	CvMat* vF = cvCreateMat(3, 3, CV_32FC1);
	cvSVD(fundamentalMatrix, dF, uF, vF);

	//--- Diagonal dF
	CvMat* diagDF = cvCreateMat(3, 3, CV_32FC1);
	cvConvertScale(diagDF, diagDF, 0);
	cvmSet(diagDF, 0, 0, cvmGet(dF, 0, 0));
	cvmSet(diagDF, 1, 1, cvmGet(dF, 1, 1));

	cvReleaseMat(&dF);

	//--- Transpose vF
	CvMat* transposeVF = cvCreateMat(3, 3, CV_32FC1);
	cvTranspose(vF, transposeVF);

	cvReleaseMat(&vF);

	// F = U*diag([D(1,1) D(2,2) 0])*V'
	cvMatMul(uF, diagDF, uF);
	cvMatMul(uF, transposeVF, fundamentalMatrix);

	cvReleaseMat(&diagDF);
	cvReleaseMat(&uF);
	cvReleaseMat(&transposeVF);

	CvMat* normalizationTransform2Transpose = cvCreateMat(3, 3, CV_32FC1);
	cvTranspose(normalizationTransform2, normalizationTransform2Transpose);

	cvReleaseMat(&normalizationTransform2);

	cvMatMul(normalizationTransform2Transpose, fundamentalMatrix, fundamentalMatrix);
	cvMatMul(fundamentalMatrix, normalizationTransform1, fundamentalMatrix);

	Utils::cvPrintMat(fundamentalMatrix, 3, 3);

	cvReleaseMat(&normalizationTransform1);

}

void BasicReconstructor::computeCameraMatrices(CvMat* p1, CvMat* p2, CvMat* fundamentalMatrix) {

	//--- First camera matrix
	cvSetIdentity(p1);

	//--- Second camera matrix
	CvMat* s = cvCreateMat(3, 3, CV_32FC1);
	CvMat* u = cvCreateMat(3, 3, CV_32FC1);
	CvMat* v = cvCreateMat(3, 3, CV_32FC1);
	cvSVD(fundamentalMatrix, s, u, v);

	//--- Get epipole
	CvMat* eColumnVector = cvCreateMat(3, 1, CV_32FC1);
	cvGetCol(u, eColumnVector, 2);

	CvMat* eContrEps = computeContractionByEpsilonTensor(eColumnVector, 3, 1);

	CvMat* fMultByEContrEps = cvCreateMat(3, 3, CV_32FC1);
	cvMatMul(eContrEps, fundamentalMatrix, fMultByEContrEps);

	for (int i = 0; i < 3; i++) {

		cvmSet(p2, i, 0, -cvmGet(fMultByEContrEps, i, 0));
		cvmSet(p2, i, 1, -cvmGet(fMultByEContrEps, i, 1));
		cvmSet(p2, i, 2, -cvmGet(fMultByEContrEps, i, 2));
		cvmSet(p2, i, 3, cvmGet(u, i, 2));

	}

	//--- Matlab is giving positive values for last column, while openCV gives negative ones.
	//--- So we multiply p2 by -1;

	cvConvertScale(p2, p2, -1);

	cvReleaseMat(&s);
	cvReleaseMat(&u);
	cvReleaseMat(&v);

	cvReleaseMat(&eColumnVector);
	cvReleaseMat(&eContrEps);
	cvReleaseMat(&fMultByEContrEps);

}

vector<POI> BasicReconstructor::computeLinearEstimationOf3dCoordinates(CvMat* p1, CvMat* p2, CvMat* points1,
		CvMat* points2, int cameraWidth, int cameraHeight, map<string, POI> firstCameraPOIs) {

	vector<POI> POIs3d;

	CvMat* h = cvCreateMat(3, 3, CV_32FC1);
	cvConvertScale(h, h, 0);
	cvmSet(h, 0, 0, 2.0f / cameraWidth);
	cvmSet(h, 1, 1, 2.0f / cameraHeight);
	cvmSet(h, 0, 2, -1.0f);
	cvmSet(h, 1, 2, -1.0f);
	cvmSet(h, 2, 2, 1.0f);

	CvMat* p1Norm = cvCreateMat(3, 4, CV_32FC1);
	cvMatMul(h, p1, p1Norm);

	CvMat* p2Norm = cvCreateMat(3, 4, CV_32FC1);
	cvMatMul(h, p2, p2Norm);

	CvMat* u = cvCreateMat(2, 2, CV_32FC1);

	int i = 0;
	for (map<string, POI>::const_iterator it = firstCameraPOIs.begin(); it != firstCameraPOIs.end(); ++it) {

		POI firstCameraPOI = it->second;

		//--- Build u (point matrix)
		cvmSet(u, 0, 0, points1->data.fl[i * 2]);
		cvmSet(u, 1, 0, points1->data.fl[i * 2 + 1]);
		cvmSet(u, 0, 1, points2->data.fl[i * 2]);
		cvmSet(u, 1, 1, points2->data.fl[i * 2 + 1]);

		//--- Normalize u
		float u00 = cvmGet(h, 0, 0) * cvmGet(u, 0, 0) + cvmGet(h, 0, 1) * cvmGet(u, 1, 0) + cvmGet(h, 0, 2);
		float u10 = cvmGet(h, 1, 0) * cvmGet(u, 0, 0) + cvmGet(h, 1, 1) * cvmGet(u, 1, 0) + cvmGet(h, 1, 2);
		float u01 = cvmGet(h, 0, 0) * cvmGet(u, 0, 1) + cvmGet(h, 0, 1) * cvmGet(u, 1, 1) + cvmGet(h, 0, 2);
		float u11 = cvmGet(h, 1, 0) * cvmGet(u, 0, 1) + cvmGet(h, 1, 1) * cvmGet(u, 1, 1) + cvmGet(h, 1, 2);

		CvMat* point1Homogeneous = cvCreateMat(3, 1, CV_32FC1);
		cvmSet(point1Homogeneous, 0, 0, u00);
		cvmSet(point1Homogeneous, 1, 0, u10);
		cvmSet(point1Homogeneous, 2, 0, 1);

		CvMat* point2Homogeneous = cvCreateMat(3, 1, CV_32FC1);
		cvmSet(point2Homogeneous, 0, 0, u01);
		cvmSet(point2Homogeneous, 1, 0, u11);
		cvmSet(point2Homogeneous, 2, 0, 1);

		CvMat* point1HomogeneousContrByEps = computeContractionByEpsilonTensor(point1Homogeneous, 3, 1);
		CvMat* point2HomogeneousContrByEps = computeContractionByEpsilonTensor(point2Homogeneous, 3, 1);

		CvMat* a1 = cvCreateMat(3, 4, CV_32FC1);
		cvMatMul(point1HomogeneousContrByEps, p1Norm, a1);

		CvMat* a2 = cvCreateMat(3, 4, CV_32FC1);
		cvMatMul(point2HomogeneousContrByEps, p2Norm, a2);

		CvMat* a1a2 = cvCreateMat(6, 4, CV_32FC1);
		for (int row = 0; row < 6; row++) {
			for (int column = 0; column < 4; column++) {

				float value = 0.0f;
				if (row < 3) {
					value = cvmGet(a1, row, column);
				} else {
					value = cvmGet(a2, row - 3, column);
				}
				cvmSet(a1a2, row, column, value);
			}
		}

		CvMat* dummyA = cvCreateMat(4, 4, CV_32FC1);
		CvMat* dummyB = cvCreateMat(6, 4, CV_32FC1);
		CvMat* x = cvCreateMat(4, 4, CV_32FC1);
		cvSVD(a1a2, dummyA, dummyB, x);

		CvMat* homogeneous3dCoordinates = cvCreateMat(4, 1, CV_32FC1);
		cvmSet(homogeneous3dCoordinates, 0, 0, cvmGet(x, 0, 3));
		cvmSet(homogeneous3dCoordinates, 1, 0, cvmGet(x, 1, 3));
		cvmSet(homogeneous3dCoordinates, 2, 0, cvmGet(x, 2, 3));
		cvmSet(homogeneous3dCoordinates, 3, 0, cvmGet(x, 3, 3));

		//--- Lets get scale parameter to 1
		cvConvertScale(homogeneous3dCoordinates, homogeneous3dCoordinates, 1.0f
				/ cvmGet(homogeneous3dCoordinates, 3, 0));

		//--- Do we need to multiply coordinates by - 1?
		firstCameraPOI.setCoordinates3d(cvPoint3D32f(cvmGet(homogeneous3dCoordinates, 0, 0), cvmGet(
				homogeneous3dCoordinates, 1, 0), cvmGet(homogeneous3dCoordinates, 2, 0)));

		firstCameraPOI.getInfo3d();

		POIs3d.push_back(firstCameraPOI);

		i++;

		cvReleaseMat(&point1Homogeneous);
		cvReleaseMat(&point2Homogeneous);
		cvReleaseMat(&point1HomogeneousContrByEps);
		cvReleaseMat(&point2HomogeneousContrByEps);
		cvReleaseMat(&a1);
		cvReleaseMat(&a2);
		cvReleaseMat(&a1a2);
		cvReleaseMat(&dummyA);
		cvReleaseMat(&dummyB);
		cvReleaseMat(&x);
		cvReleaseMat(&homogeneous3dCoordinates);

	}

	cvReleaseMat(&h);
	cvReleaseMat(&p1Norm);
	cvReleaseMat(&p2Norm);
	cvReleaseMat(&u);

	return POIs3d;
}

CvMat* BasicReconstructor::computeContractionByEpsilonTensor(CvMat* mat, int rows, int columns) {

	CvMat* result = NULL;

	if ((rows == 3 && columns == 1) || (rows == 1 && columns == 3)) {

		bool rowVector = false;
		if (rows == 3) {
			rowVector = true;
		}

		result = cvCreateMat(3, 3, CV_32FC1);
		cvSet(result, cvScalar(0));

		float element1 = 0.0f;
		float element2 = 0.0f;
		float element3 = 0.0f;

		if (rowVector) {
			element1 = cvmGet(mat, 0, 0);
			element2 = cvmGet(mat, 1, 0);
			element3 = cvmGet(mat, 2, 0);
		} else {
			element1 = cvmGet(mat, 0, 0);
			element2 = cvmGet(mat, 0, 1);
			element3 = cvmGet(mat, 0, 2);
		}

		//--- First Line
		cvmSet(result, 0, 1, element3);
		cvmSet(result, 0, 2, -element2);

		//--- Second Line
		cvmSet(result, 1, 0, -element3);
		cvmSet(result, 1, 2, element1);

		//--- Third Line
		cvmSet(result, 2, 0, element2);
		cvmSet(result, 2, 1, -element1);

	} else {
		logERROR("Unrecognized contraction by epsilon tensor!");
	}

	return result;
}

CvMat* BasicReconstructor::computeNormalizedPoints(CvMat *points, int nPoints, CvMat* normalizationTransform) {

	CvMat* normalizedPoints = cvCreateMat(1, nPoints, CV_32FC2);

	float meanX = 0;
	float meanY = 0;

	for (int i = 0; i < nPoints; i++) {
		meanX += points->data.fl[i * 2];
		meanY += points->data.fl[i * 2 + 1];

	}
	meanX /= nPoints;
	meanY /= nPoints;

	float meanDist = 0;
	for (int i = 0; i < nPoints; i++) {
		normalizedPoints->data.fl[i * 2] = points->data.fl[i * 2] - meanX;
		normalizedPoints->data.fl[i * 2 + 1] = points->data.fl[i * 2 + 1] - meanY;
		meanDist
				+= cvSqrt(powf(normalizedPoints->data.fl[i * 2], 2.0f) + powf(normalizedPoints->data.fl[i * 2 + 1], 2.0f));
	}
	meanDist /= nPoints;

	float scale = cvSqrt(2.0f) / meanDist;
	for (int i = 0; i < nPoints; i++) {
		normalizedPoints->data.fl[i * 2] = scale * points->data.fl[i * 2] - (scale * meanX);
		normalizedPoints->data.fl[i * 2 + 1] = scale * points->data.fl[i * 2 + 1] - (scale * meanY);
	}

	cvConvertScale(normalizationTransform, normalizationTransform, 0.0f);
	cvmSet(normalizationTransform, 0, 0, scale);
	cvmSet(normalizationTransform, 0, 2, -(scale * meanX));
	cvmSet(normalizationTransform, 1, 1, scale);
	cvmSet(normalizationTransform, 1, 2, -(scale * meanY));
	cvmSet(normalizationTransform, 2, 2, 1.0f);

	return normalizedPoints;
}
