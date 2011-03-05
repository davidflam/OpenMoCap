#ifndef CALIBRATIONRESULTENUM_H_
#define CALIBRATIONRESULTENUM_H_

#include "AbstractEnum.h"

class CalibrationResultEnum: public AbstractEnum {

public:

	/*!
	 * CalibrationResultEnum constructor.
	 *
	 * @param text Text that represents the enumeration content.
	 */
	CalibrationResultEnum(string text) :
		AbstractEnum(text) {
	}

	/*!
	 * Inserts the item "SUCCESS" into the enumeration.
	 */
	const static CalibrationResultEnum SUCCESS;

	/*!
	 * Inserts the item "NO_VALID_IMAGE" into the enumeration.
	 */
	const static CalibrationResultEnum NO_VALID_IMAGE;
};

#endif /* CALIBRATIONRESULTENUM_H_ */
