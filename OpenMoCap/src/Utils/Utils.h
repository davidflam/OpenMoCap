/*!
 * Utils class
 * Contains static utility methods
 *
 * \name Utils
 * \author David Lunardi Flam
 * \version
 * \since 18/02/2009
 */
#ifndef UTILS_H_
#define UTILS_H_

#include <opencv2/opencv.hpp>
#include <sstream>
#include <limits>
#include <iostream>

class Utils {

public:

	/*!
	 * Returns distance between two bidimensional coordinates
	 * @param p1 first point
	 * @param p2 second point
	 * @return
	 */
	static double cvPointDist(CvPoint p1, CvPoint p2) {

		double x = p2.x - p1.x;
		double y = p2.y - p1.y;

		return sqrt((x * x) + (y * y));
	}

	/*!
	 * Returns distance between two bidimensional coordinates
	 * @param p1 first point
	 * @param p2 second point
	 * @return
	 */
	static float cvPoint2d32fDist(CvPoint2D32f& p1, CvPoint2D32f& p2) {

		float x = p2.x - p1.x;
		float y = p2.y - p1.y;

		return sqrt((x * x) + (y * y));
	}

	/*!
	 * Converts some objet to string with stringstream
	 * @param object
	 * @return string representation
	 */
	template <typename T>
	static string toString(T object) {

		string result;
		stringstream stringStream;

		stringStream << object;
		stringStream >> result;

		return result;

	}

	/*!
	 * Prints a matrix
	 */
	static void cvPrintMat(CvMat* mat, int rows, int columns) {

		cout << endl;
		for (int i = 0; i < rows; i ++) {
			for (int j = 0; j < columns; j ++) {
				cout << cvmGet(mat, i, j) << " ";
			}
			cout << endl;
		}
		cout << endl;
	}


	static float sum(float** vec, int size, int dimension){
		float Sum = 0;
		for (int i=0; i<size; i++)
		{
			Sum += vec[i][dimension];
			//printf("Sum %i, %f\n", i, Sum);
		}
		//printf("Sum Final%f\n", Sum);
		return Sum;
	}

	static float getMean(float** vec, int size, int dimension)
	{
		float mean=0;

		mean = sum(vec, size, dimension )/size;

		//printf("Mean: %f\n", mean);

		return	mean;
	}

	static float sqm(float** vec, int size, int dimension){
		float Mean, Sqm, value = 0;
		Sqm = 0.0f;
		Mean = getMean(vec, size, dimension);

		for (int i=0; i<size; i++){ // Otimizar
			value = vec[i][dimension] - Mean;
			Sqm += value * value;
		}

		return Sqm;
	}

	static float getVariance(float** vec, int size, int dimension){
		float Sqm;
		Sqm = sqm(vec, size, dimension);
		//printf("SQM %f \n", Sqm);

		return ( Sqm / (size-1) );
	}


};
/*
class sampleStat
{
public:
		float* sampleSet;
		int N;
		float Sum;

		float sum()
		{
			Sum = 0;
			for (int i=0; i<N; i++)
			{
				Sum += sampleSet[i];
				//printf("Sum %i, %f\n", i, Sum);
			}
			//printf("Sum Final%f\n", Sum);
			return Sum;
		}

		float sqm()
		{
			float Mean, Sqm;
			Sqm = 0;
			Mean = getMean();
			for (int i=0; i<N; i++) // Otimizar
				Sqm+=(sampleSet[i]-Mean)*(sampleSet[i]-Mean);
			return Sqm;
		}

		void setValue (float x, int i){
			sampleSet[i]=x;
			//printf("SampleSet[i]:%f\n", sampleSet[i]);
		}

		sampleStat(int n){
			N = n;
			sampleSet = new float[N];
		}

		float getMean()
		{
			float mean=0;

			mean = sum()/N;

			//printf("Mean: %f\n", mean);

			return	mean;
		}

		float getVariance(){
			float Sqm;
			Sqm = sqm();
			//printf("SQM %f \n", Sqm);

			return Sqm/(N-1);
		}

		float getStdDeviation(){
			return sqrt(getVariance());
		}
};
*/

#endif /* UTILS_H_ */

