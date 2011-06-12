/*!
 * Class that contains functions to help the program debug.
 *
 * \name Debug
 * \author David Lunardi Flam
 * \version
 * \since 07/16/2008
 */
#include "Debug.h"

Debug::Debug() {

}

Debug::~Debug() {

}

void Debug::printNewLine(int n) {

	for (int i = 0; i < n; i++) {
		cout << endl;
	}
}

void Debug::printSpace(int n) {

	for (int i = 0; i < n; i++) {
		cout << " ";
	}
}

void Debug::printCvPoint2D32f(CvPoint2D32f point) {

	cout << " (" << point.x << "," << point.y << ")";
}

void Debug::printArrayCvPoint2D32f(CvPoint2D32f *array, int size) {

	cout << "Array = { ";
	for (int i = 0; i < size; i++) {
		printCvPoint2D32f(array[i]);
		if (i % 8 == 0) {
			printNewLine();
		}
	}
	cout << " }" << endl;
}

void Debug::printCvPoint3D32f(CvPoint3D32f point) {

	cout << " (" << point.x << "," << point.y << "," << point.z << ")";
}

void Debug::printArrayCvPoint3D32f(CvPoint3D32f *array, int size) {

	cout << "Array = { ";
	for (int i = 0; i < size; i++) {
		printCvPoint3D32f(array[i]);
		if (i % 8 == 0) {
			printNewLine();
		}
	}
	cout << " }" << endl;
}

void Debug::printCvMat(CvMat* mat, string name) {

	float x = 0, y = 0, z = 0;
    string cvMatLineInfo = "";

    logDEBUG("%s", name.c_str());
    logDEBUG("Rows = %d X Cols = %d", mat->rows, mat->cols);

	for (int i = 0; i < mat->rows; i++) {
		cvMatLineInfo = "";
		for (int j = 0; j < mat->cols; j++) {

			switch (CV_MAT_TYPE(mat->type)) {
				case CV_MAT_TYPE(CV_32SC1):
					cvMatLineInfo += " " + stringify(mat->data.i[ (i * mat->rows) + j] );
					break;
				case CV_MAT_TYPE(CV_64FC1):
					cvMatLineInfo += " " + stringify(cvmGet(mat, i, j));
					break;
				case CV_MAT_TYPE(CV_32FC2):
					x = mat->data.fl[ (i * mat->rows) + (j * 2) + 0];
					y = mat->data.fl[ (i * mat->rows) + (j * 2) + 1];
					cvMatLineInfo += " (" + stringify(x) + ", " + stringify(y) + ")";
					break;
				case CV_MAT_TYPE(CV_32FC3):
					x = mat->data.fl[ (i * mat->rows) + (j * 3) + 0];
					y = mat->data.fl[ (i * mat->rows) + (j * 3) + 1];
					z = mat->data.fl[ (i * mat->rows) + (j * 3) + 2];
					cvMatLineInfo += " (" + stringify(x) + ", " + stringify(y) + ", " + stringify(z) + ")";
					break;
				default:
					logDEBUG("Unsupported CV_MAT_TYPE = %d", CV_MAT_TYPE(mat->type));
					logDEBUG("Known CV_MAT_TYPES");
					logDEBUG("CV_32SC1 = %d", CV_MAT_TYPE(CV_32SC1));
					logDEBUG("CV_64FC1 = %d", CV_MAT_TYPE(CV_64FC1));
					logDEBUG("CV_32FC2 = %d", CV_MAT_TYPE(CV_32FC2));
					logDEBUG("CV_32FC3 = %d", CV_MAT_TYPE(CV_32FC3));
					break;
			}

		}
		logDEBUG("%s", cvMatLineInfo.c_str());
	}

}
