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
