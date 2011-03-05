#ifndef TRACKINGALGORITHMENUM_H_
#define TRACKINGALGORITHMENUM_H_

#include "AbstractEnum.h"

class TrackingAlgorithmEnum: public AbstractEnum {

public:

	/*!
	 * TrackingAlgorithmEnum constructor.
	 *
	 * @param text Text that represents the enumeration content.
	 */
	TrackingAlgorithmEnum(string text) :
		AbstractEnum(text) {
	}

	/*!
	 * Inserts the item "ALPHA_BETA_GAMMA_FILTER" and "KALMAN_FILTER" into the enumeration.
	 */
	const static TrackingAlgorithmEnum ALPHA_BETA_GAMMA_FILTER;
	const static TrackingAlgorithmEnum KALMAN_FILTER;
	const static TrackingAlgorithmEnum CONDENSATION;
};

#endif /* TRACKINGALGORITHMENUM_H_ */
