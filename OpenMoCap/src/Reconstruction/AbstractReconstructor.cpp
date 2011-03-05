/*!
 * Abstract class for reconstruction algorithms.
 *
 * \name AbstractReconstructor
 * \author David Lunardi Flam
 * \version
 * \since 03/14/2009
 */
#include "AbstractReconstructor.h"

void AbstractReconstructor::computePointArrays(map<string, POI> firstCameraPOIs, map<string, POI> secondCameraPOIs,
		CvMat* points1, CvMat* points2, int width, int height) {

	int i = 0;
	for (map<string, POI>::const_iterator it = firstCameraPOIs.begin(); it != firstCameraPOIs.end(); ++it) {
		POI firstCameraPOI = it->second;

		//--- Image coordinates are upside down

		points1->data.fl[i * 2] = (width - 1) - firstCameraPOI.getCoordinates2d().x;
		points1->data.fl[i * 2 + 1] = (height - 1) - firstCameraPOI.getCoordinates2d().y;

		POI secondCameraPOI = secondCameraPOIs[it->second.getSemantic().toStdString()];
		points2->data.fl[i * 2] = (width - 1) - secondCameraPOI.getCoordinates2d().x;
		points2->data.fl[i * 2 + 1] = (height - 1) - secondCameraPOI.getCoordinates2d().y;

		i++;
	}

}

void AbstractReconstructor::computePointArrays(vector<map<string, POI> >& CameraPOIs, vector<CvMat*>& points,
		int width, int height) {

	int i = 0;
	POI tempPOI;

	for (map<string, POI>::const_iterator it = CameraPOIs[0].begin(); it != CameraPOIs[0].end(); ++it) {
		string namePOI = it->second.getSemantic().toStdString();
		for (unsigned int cam = 0; cam < CameraPOIs.size(); cam++) {
			/* Reconstruction Model (0,0) = down-left
			 * OpenCV Image (0,0) = up-left
			 * Transforms OpenCV image coordinates into our reconstruction model image coordinates
			 */
			tempPOI = CameraPOIs[cam][namePOI];
			points[cam]->data.fl[i * 2] = (width - 1) - tempPOI.getCoordinates2d().x;
			points[cam]->data.fl[i * 2 + 1] = (height - 1) - tempPOI.getCoordinates2d().y;

		}
		i++;
	}

}

CvMat* AbstractReconstructor::compute3dHomogeneousCoordinates(CvMat* projectionMatrix1, CvMat* projectionMatrix2,
		CvPoint2D32f point1, CvPoint2D32f point2) {

	CvMat* A = cvCreateMat(4, 4, CV_32FC1);

	//---Linear LS Triangulation
	cvmSet(A, 0, 0, (point1.x * cvmGet(projectionMatrix1, 2, 0) - cvmGet(projectionMatrix1, 0, 0)));
	cvmSet(A, 0, 1, (point1.x * cvmGet(projectionMatrix1, 2, 1) - cvmGet(projectionMatrix1, 0, 1)));
	cvmSet(A, 0, 2, (point1.x * cvmGet(projectionMatrix1, 2, 2) - cvmGet(projectionMatrix1, 0, 2)));
	cvmSet(A, 0, 3, (point1.x * cvmGet(projectionMatrix1, 2, 3) - cvmGet(projectionMatrix1, 0, 3)));

	cvmSet(A, 1, 0, (point1.y * cvmGet(projectionMatrix1, 2, 0) - cvmGet(projectionMatrix1, 1, 0)));
	cvmSet(A, 1, 1, (point1.y * cvmGet(projectionMatrix1, 2, 1) - cvmGet(projectionMatrix1, 1, 1)));
	cvmSet(A, 1, 2, (point1.y * cvmGet(projectionMatrix1, 2, 2) - cvmGet(projectionMatrix1, 1, 2)));
	cvmSet(A, 1, 3, (point1.y * cvmGet(projectionMatrix1, 2, 3) - cvmGet(projectionMatrix1, 1, 3)));

	cvmSet(A, 2, 0, (point2.x * cvmGet(projectionMatrix2, 2, 0) - cvmGet(projectionMatrix2, 0, 0)));
	cvmSet(A, 2, 1, (point2.x * cvmGet(projectionMatrix2, 2, 1) - cvmGet(projectionMatrix2, 0, 1)));
	cvmSet(A, 2, 2, (point2.x * cvmGet(projectionMatrix2, 2, 2) - cvmGet(projectionMatrix2, 0, 2)));
	cvmSet(A, 2, 3, (point2.x * cvmGet(projectionMatrix2, 2, 3) - cvmGet(projectionMatrix2, 0, 3)));

	cvmSet(A, 3, 0, (point2.y * cvmGet(projectionMatrix2, 2, 0) - cvmGet(projectionMatrix2, 1, 0)));
	cvmSet(A, 3, 1, (point2.y * cvmGet(projectionMatrix2, 2, 1) - cvmGet(projectionMatrix2, 1, 1)));
	cvmSet(A, 3, 2, (point2.y * cvmGet(projectionMatrix2, 2, 2) - cvmGet(projectionMatrix2, 1, 2)));
	cvmSet(A, 3, 3, (point2.y * cvmGet(projectionMatrix2, 2, 3) - cvmGet(projectionMatrix2, 1, 3)));

	CvMat* U = cvCreateMat(4, 4, CV_32FC1);
	CvMat* W = cvCreateMat(4, 1, CV_32FC1);
	CvMat* V = cvCreateMat(4, 4, CV_32FC1);

	cvSVD(A, W, U, V, CV_SVD_V_T);

	CvMat* x = cvCreateMat(4, 1, CV_32FC1);
	cvmSet(x, 0, 0, V->data.fl[12] / V->data.fl[15]);
	cvmSet(x, 1, 0, V->data.fl[13] / V->data.fl[15]);
	cvmSet(x, 2, 0, V->data.fl[14] / V->data.fl[15]);
	cvmSet(x, 3, 0, 1.0f);

	cvReleaseMat(&W);
	cvReleaseMat(&U);
	cvReleaseMat(&V);
	cvReleaseMat(&A);

	return x;

}

CvMat* AbstractReconstructor::compute3dHomogeneousCoordinates(vector<CvMat*>& projectionMatrix,
		vector<CvPoint2D32f>& points) {

	int numCam = projectionMatrix.size();
	int m = 2 * numCam;
	int n = 4;

	CvMat* A = cvCreateMat(m, n, CV_32FC1);

	for (int i = 0; i < numCam; i++) {
		cvmSet(A, 2 * i, 0, (points[i].x * cvmGet(projectionMatrix[i], 2, 0) - cvmGet(projectionMatrix[i], 0, 0)));
		cvmSet(A, 2 * i, 1, (points[i].x * cvmGet(projectionMatrix[i], 2, 1) - cvmGet(projectionMatrix[i], 0, 1)));
		cvmSet(A, 2 * i, 2, (points[i].x * cvmGet(projectionMatrix[i], 2, 2) - cvmGet(projectionMatrix[i], 0, 2)));
		cvmSet(A, 2 * i, 3, (points[i].x * cvmGet(projectionMatrix[i], 2, 3) - cvmGet(projectionMatrix[i], 0, 3)));

		cvmSet(A, (2 * i) + 1, 0, (points[i].y * cvmGet(projectionMatrix[i], 2, 0) - cvmGet(projectionMatrix[i], 1, 0)));
		cvmSet(A, (2 * i) + 1, 1, (points[i].y * cvmGet(projectionMatrix[i], 2, 1) - cvmGet(projectionMatrix[i], 1, 1)));
		cvmSet(A, (2 * i) + 1, 2, (points[i].y * cvmGet(projectionMatrix[i], 2, 2) - cvmGet(projectionMatrix[i], 1, 2)));
		cvmSet(A, (2 * i) + 1, 3, (points[i].y * cvmGet(projectionMatrix[i], 2, 3) - cvmGet(projectionMatrix[i], 1, 3)));
	}

	CvMat* W = cvCreateMat(n, 1, CV_32FC1);
	CvMat* V = cvCreateMat(n, n, CV_32FC1);

	cvSVD(A, W, NULL, V, CV_SVD_MODIFY_A | CV_SVD_V_T);

	CvMat* X = cvCreateMat(n, 1, CV_32FC1);
	cvmSet(X, 0, 0, V->data.fl[12] / V->data.fl[15]);
	cvmSet(X, 1, 0, V->data.fl[13] / V->data.fl[15]);
	cvmSet(X, 2, 0, V->data.fl[14] / V->data.fl[15]);
	cvmSet(X, 3, 0, 1.0f);

	cvReleaseMat(&W);
	cvReleaseMat(&V);
	cvReleaseMat(&A);

	return X;

}
