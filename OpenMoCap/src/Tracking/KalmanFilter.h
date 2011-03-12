#ifndef KALMANFILTER_H_
#define KALMANFILTER_H_

#include "AbstractTracker.h"

class KalmanFilter: public AbstractTracker {

public:

	/*!
	 * KalmanFilter constructor.
	 */
	KalmanFilter();

	/*!
	 * KalmanFilter destructor.
	 */
	~KalmanFilter();

	/*!
	 * Initializes the KalmanFilter.
	 */
	CvKalman* initKalman(CvPoint2D32f initCoord);

	/*!
	 * \see AbstractTracker::refreshPOIsPosition
	 */
	void refreshPOIsPosition(map<string, POI>& currentPOIs, vector<POI>& detectedPOIs, int maxCoordX, int maxCoordY, int &radius);

	void restartTracker();

};

#endif /* KALMANFILTER_H_ */
