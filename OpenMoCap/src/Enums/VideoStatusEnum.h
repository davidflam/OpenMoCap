#ifndef VIDEOSTATUSENUM_H_
#define VIDEOSTATUSENUM_H_

#include "AbstractEnum.h"

class VideoStatusEnum: public AbstractEnum {

public:

	/*!
	 * VideoStatusEnum constructor.
	 *
	 * @param text Text that represents the enumeration content.
	 */
	VideoStatusEnum(string text) :
		AbstractEnum(text) {
	}

	/*!
	 * Inserts the item "PLAY" into the enumeration.
	 */
	const static VideoStatusEnum PLAY_LIVE;

	/*!
	 * Inserts the item "PLAY" into the enumeration.
	 */
	const static VideoStatusEnum PLAY_VIDEO;

	/*!
	 * Inserts the item "STOP" into the enumeration.
	 */
	const static VideoStatusEnum STOP;

	/*!
	 * Inserts the item "RECORD" into the enumeration.
	 */
	const static VideoStatusEnum RECORD;

	/*!
	 * Inserts the item "CALIBRATE" into the enumeration.
	 */
	const static VideoStatusEnum CALIBRATE;

};

#endif /* VIDEOSTATUSENUM_H_ */
