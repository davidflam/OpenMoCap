/*!
 * Alpha-Beta-Gamma Filter
 *
 * \name AlphaBetaGammaFilter
 * \author David Lunardi Flam
 * \version
 * \since 02/18/2009
 */
#include "AlphaBetaGammaFilter.h"

AlphaBetaGammaFilter::AlphaBetaGammaFilter() {

}

AlphaBetaGammaFilter::~AlphaBetaGammaFilter() {

}

void AlphaBetaGammaFilter::refreshPOIsPosition(map<string, POI>& currentPOIs, vector<POI>& detectedPOIs, int maxCoordX,
		int maxCoordY, int &radius) {

	//--- For every initialized POI
	for (map<string, POI>::const_iterator it = currentPOIs.begin(); it != currentPOIs.end(); ++it) {

		POI POIToTrack = it->second;

		CvPoint2D32f predictedVelocity = POIToTrack.getVelocityVector2d();
		CvPoint2D32f predictedPosition = POIToTrack.getPredictedPosition();
		predictedPosition.x += predictedVelocity.x;
		predictedPosition.y += predictedVelocity.y;

		if (predictedPosition.x < 0) {
			predictedPosition.x = 0;
		} else if (predictedPosition.x > maxCoordX) {
			predictedPosition.x = maxCoordX;
		}

		if (predictedPosition.y < 0) {
			predictedPosition.y = 0;
		} else if (predictedPosition.y > maxCoordY) {
			predictedPosition.y = maxCoordY;
		}

		//--- Based on predicted position, let's find the nearest detected POI without semantic
		double minDist = std::numeric_limits<double>::max();
		int minDistIndex = -1;

		CvPoint2D32f maxCoord = cvPoint2D32f(maxCoordX, maxCoordY);
		CvPoint2D32f origin = cvPoint2D32f(0.0f, 0.0f);
		double percentMaxDist = 0.05;
		double maxDist = Utils::cvPoint2d32fDist(origin, maxCoord) * percentMaxDist;
		radius = maxDist;

		for (unsigned int i = 0; i < detectedPOIs.size(); i++) {

			CvPoint2D32f newPosition = detectedPOIs[i].getCoordinates2d();
			double distToNewPosition = Utils::cvPoint2d32fDist(predictedPosition, newPosition);

			//Search region is the circle with radius maxDist
			if (distToNewPosition < minDist && distToNewPosition < maxDist) {

				minDist = distToNewPosition;
				minDistIndex = i;
			}

		}

		cout << "minDist: " << minDist << endl;

		//--- Found correspondence
		if (minDistIndex >= 0) {

			CvPoint2D32f measuredPosition = detectedPOIs[minDistIndex].getCoordinates2d();

			CvPoint2D32f residualError;
			residualError.x = measuredPosition.x - predictedPosition.x;
			residualError.y = measuredPosition.y - predictedPosition.y;

			float alfa = 1.0;
			CvPoint2D32f smoothedPosition;
			smoothedPosition.x = predictedPosition.x + (alfa * residualError.x);
			smoothedPosition.y = predictedPosition.y + (alfa * residualError.y);
			POIToTrack.setPredictedPosition(smoothedPosition);

			float beta = 0.8;
			CvPoint2D32f smoothedVelocity;
			smoothedVelocity.x = predictedVelocity.x + (beta * residualError.x);
			smoothedVelocity.y = predictedVelocity.y + (beta * residualError.y);
			POIToTrack.setVelocityVector2d(smoothedVelocity);

			//--- Refreshes POI position
			POIToTrack.setCoordinates2d(measuredPosition);

			currentPOIs[POIToTrack.getSemantic().toStdString()] = POIToTrack;

			//--- Removes used detected POI
			std::swap(detectedPOIs[minDistIndex], detectedPOIs[detectedPOIs.size() - 1]);
			detectedPOIs.pop_back();

		//--- Inertia
		} else {

			POIToTrack.setCoordinates2d(predictedPosition);
			POIToTrack.setPredictedPosition(predictedPosition);
			POIToTrack.setVelocityVector2d(POIToTrack.getVelocityVector2d());
			currentPOIs[POIToTrack.getSemantic().toStdString()] = POIToTrack;

		}

	}
}

void AlphaBetaGammaFilter::restartTracker(){


}
