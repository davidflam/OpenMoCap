#ifndef ALPHABETAGAMMAFILTER_H_
#define ALPHABETAGAMMAFILTER_H_

#include "AbstractTracker.h"
#include "../Utils/Utils.h"

class AlphaBetaGammaFilter: public AbstractTracker {

public:

	/*!
	 * AlphaBetaGammaFilter constructor.
	 */
	AlphaBetaGammaFilter();

	/*!
	 * AlphaBetaGammaFilter destructor.
	 */
	~AlphaBetaGammaFilter();

	/*!
	 * \see AbstractTracker::refreshPOIsPosition
	 */
	void refreshPOIsPosition(map<string, POI>& currentPOIs, vector<POI>& detectedPOIs, int maxCoordX, int maxCoordY, int &radius);

	void restartTracker();

};

#endif /* ALPHABETAGAMMAFILTER_H_ */
