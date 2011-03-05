#ifndef CALIBRATIONALGORITHMENUM_H_
#define CALIBRATIONALGORITHMENUM_H_

#include "AbstractEnum.h"

class CalibrationAlgorithmEnum: public AbstractEnum {

public:

	/*!
	 * CalibrationAlgorithmEnum constructor.
	 *
	 * @param text Text that represents the enumeration content.
	 */
	CalibrationAlgorithmEnum(string text) :
		AbstractEnum(text) {
	}

	/*!
	 * Inserts the item "ZHANG" into the enumeration.
	 */
	const static CalibrationAlgorithmEnum ZHANG;
};

#endif /* CALIBRATIONALGORITHMENUM_H_ */
