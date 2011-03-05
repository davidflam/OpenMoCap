#ifndef CENTROIDFINDERALGORITHMENUM_H_
#define CENTROIDFINDERALGORITHMENUM_H_

#include "AbstractEnum.h"

class POIFinderAlgorithmEnum: public AbstractEnum {

public:

	/*!
	 * CalibrationResultEnum constructor.
	 *
	 * @param text Text that represents the enumeration content.
	 */
	POIFinderAlgorithmEnum(string text) :
		AbstractEnum(text) {
	}

	/*!
	 * Inserts the item "BLOB_EXTRACTOR_6_CONNECTIVITY" into the enumeration.
	 */
	const static POIFinderAlgorithmEnum BLOB_EXTRACTOR_6_CONNECTIVITY;

	/*!
	 * Inserts the item "CHESSBOARD_CORNER_FINDER" into the enumeration
	 */
	const static POIFinderAlgorithmEnum CHESSBOARD_CORNER_FINDER;
};

#endif /* POIFINDERALGORITHMENUM_H_ */
