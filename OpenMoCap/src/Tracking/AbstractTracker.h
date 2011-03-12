#ifndef ABSTRACTTRACKER_H_
#define ABSTRACTTRACKER_H_

#include <map>
#include <string>

#include "AbstractPOIFinder.h"

class AbstractTracker {

public:

	/*!
	 * AbstractTracker destructor.
	 */
	virtual ~AbstractTracker();

	/*!
	 * Refreshes tracked POIs positions. In order to create a Tracker, one must provide
	 * an implementation for this method. Every refreshed POI must be removed from detectedPOIs vector
	 *
	 * @param currentPOIs (reference to a map of initialized POIs in current frame)
	 * @param detectedPOIs (reference to a vector of detected POIs in next frame)
	 * @param maxCoordX maximum value for x coordinate
	 * @param maxCoordY maximum value for y coordinate
	 * @param radius return the radius of the area of search (necessary to tests)
	 */
	virtual void refreshPOIsPosition(map<string, POI>& currentPOIs, vector<POI>& detectedPOIs, int maxCoordX, int maxCoordY, int &radius) = 0;

	void restartTracker();

	void test(AbstractPOIFinder* poiFinder, string filePath);



protected:
	/*!
	 * AbstractTracker protected constructor. This is an abstract class.
	 */
	AbstractTracker();

};

#endif /* ABSTRACTTRACKER_H_ */
