/*
 * Individual.h
 *
 *  Created on: 26/03/2009
 *      Author: David
 */

#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_

#include <opencv2/opencv.hpp>
#include <iostream>

#include "../Utils/Log.h"

using namespace std;

class Individual {

public:
	Individual(int numVar);

	Individual();

	~Individual();

	void info();

	void releaseMatrices();

	CvMat *getVariables() const {
		return _variables;
	}

	void setVariables(CvMat *_variables) {
		this->_variables = _variables;
	}

private:

	//--- Tx, Ty, Tz, Tx', Ty', Tz', a, b, g, a', b', g', f
	CvMat* _variables;

	double _evaluation;

};

#endif /* INDIVIDUAL_H_ */
