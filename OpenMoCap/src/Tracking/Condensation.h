#ifndef CONDENSATION_H_
#define CONDENSATION_H_

#include "AbstractTracker.h"
#include "../Utils/Utils.h"

class Condensation: public AbstractTracker {

public:

	/*!
	 * Condensation constructor.
	 */
	Condensation();

	/*!
	 * Condensation destructor.
	 */
	~Condensation();

	/*!
	 * Initializes the Condensation.
	 */
	CvConDensation* initCondensation (CvPoint2D32f initCoord, int nSample, int maxWidth, int maxHeight );

	/*!
	 * \see AbstractTracker::refreshPOIsPosition
	 */
	void refreshPOIsPosition(map<string, POI>& currentPOIs, vector<POI>& detectedPOIs, int maxCoordX, int maxCoordY, int &radius);


	CvPoint2D32f updateCondensation ( CvConDensation* conDens, CvPoint2D32f Measurement, float * stdDX_ptr, float * stdDY_ptr);

	void restartTracker();

};

#endif /* CONDENSATION_H_ */
