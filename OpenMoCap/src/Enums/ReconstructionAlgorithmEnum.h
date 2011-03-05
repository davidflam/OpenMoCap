/*
 * ReconstructionAlgorithmEnum.h
 *
 *  Created on: 14/03/2009
 *      Author: David
 */

#ifndef RECONSTRUCTIONALGORITHMENUM_H_
#define RECONSTRUCTIONALGORITHMENUM_H_

#include "AbstractEnum.h"

class ReconstructionAlgorithmEnum: public AbstractEnum {

public:

	ReconstructionAlgorithmEnum(string text) :
		AbstractEnum(text) {
	}

	const static ReconstructionAlgorithmEnum BASIC_RECONSTRUCTOR;
};

#endif /* RECONSTRUCTIONALGORITHMENUM_H_ */
