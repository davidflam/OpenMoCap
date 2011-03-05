/*
 * Individual.cpp
 *
 *  Created on: 26/03/2009
 *      Author: David
 */

#include "Individual.h"

Individual::Individual(int numVar) {

	//--- Tx, Ty, Tz, Tx', Ty', Tz', a, b, g, a', b', g', f
	_variables = cvCreateMat(1, numVar, CV_32FC1);
}

Individual::Individual() {

	//--- Tx, Ty, Tz, Tx', Ty', Tz', a, b, g, a', b', g', f
	_variables = cvCreateMat(1, 13, CV_32FC1);
}

Individual::~Individual() {

}

void Individual::info() {

	Log::getInstance()->setDefaultSeverity(pantheios::SEV_INFORMATIONAL);
	log("Tx, Ty, Tz = %f, %f, %f", _variables->data.fl[0], _variables->data.fl[1], _variables->data.fl[2]);
	//cout << "Tx, Ty, Tz = ";
	//cout << _variables->data.fl[0] << ", " << _variables->data.fl[1] << ", " << _variables->data.fl[2] << endl;
	log("Tx', Ty', Tz' = %f, %f, %f", _variables->data.fl[3], _variables->data.fl[4], _variables->data.fl[5]);
	//cout << "Tx',Ty',Tz' = ";
	//cout << _variables->data.fl[3] << ", " << _variables->data.fl[4] << ", " << _variables->data.fl[5] << endl;
	log("a, b, g = %f, %f, %f", _variables->data.fl[6], _variables->data.fl[7], _variables->data.fl[8]);
	//cout << "a, b, g = ";
	//cout << _variables->data.fl[6] << ", " << _variables->data.fl[7] << ", " << _variables->data.fl[8] << endl;
	log("a', b', g' = %f, %f, %f", _variables->data.fl[9], _variables->data.fl[10], _variables->data.fl[11]);
	//cout << "a', b', g' = ";
	//cout << _variables->data.fl[9] << ", " << _variables->data.fl[10] << ", " << _variables->data.fl[11] << endl;
	log("f = %f", _variables->data.fl[12]);
	//cout << "f = " << _variables->data.fl[12] << endl;

}

void Individual::releaseMatrices() {

	cvReleaseMat(&_variables);
}
