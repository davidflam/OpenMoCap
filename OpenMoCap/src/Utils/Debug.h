#ifndef DEBUG_H_
#define DEBUG_H_

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

class Debug {

public:

	/*!
	 * Debug constructor.
	 */
	Debug();

	/*!
	 * Debug destructor.
	 */
	~Debug();

	/*!
	 * Prints spaces.
	 *
	 * @param n Number of spaces to be printed.
	 */
	static void printSpace(int n = 1);

	/*!
	 * Prints lines.
	 *
	 * @param n Number of lines to be printed.
	 */
	static void printNewLine(int n = 1);

	//--- Functions to debug structures that use OpenCV.
	/*!
	 * Prints the OpenCV structure that represents a 2D point.
	 *
	 * @param point 2D point structure to be printed.
	 */
	static void printCvPoint2D32f(CvPoint2D32f point);

	/*!
	 * Prints the array of OpenCV structures that represent 2D points.
	 *
	 * @param array The array of 2D points structures to be printed.
	 * @param size The array size.
	 */
	static void printArrayCvPoint2D32f(CvPoint2D32f *array, int size);

	/*!
	 * Prints the OpenCV structure that represents a 3D point.
	 *
	 * @param point 3D point structure to be printed.
	 */
	static void printCvPoint3D32f(CvPoint3D32f point);

	/*!
	 * Prints the array of OpenCV structures that represent 3D points.
	 *
	 * @param array The array of 3D points structures to be printed.
	 * @param size The array size.
	 */
	static void printArrayCvPoint3D32f(CvPoint3D32f *array, int size);
};

#endif /* DEBUG_H_ */
