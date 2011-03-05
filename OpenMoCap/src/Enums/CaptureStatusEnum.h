#ifndef CAPTURESTATUSENUM_H_
#define CAPTURESTATUSENUM_H_

#include "AbstractEnum.h"

class CaptureStatusEnum: public AbstractEnum {

public:

	/*!
	 * CaptureStatusEnum constructor.
	 *
	 * @param text Text that represents the enumeration content.
	 */
	CaptureStatusEnum(string text) :
		AbstractEnum(text) {
	}

	/*!
	 * Inserts the item "RECORDING" into the enumeration.
	 */
	const static CaptureStatusEnum RECORDING;

	/*!
	 * Inserts the item "PLAYING" into the enumeration.
	 */
	const static CaptureStatusEnum PLAYING;

	/*!
	 * Inserts the item "STOP" into the enumeration.
	 */
	const static CaptureStatusEnum STOP;

};

#endif /* CAPTURESTATUSENUM_H_ */
