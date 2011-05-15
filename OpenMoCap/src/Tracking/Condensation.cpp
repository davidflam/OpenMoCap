/*!
 * CONDENSATION — Conditional Density Propagation for Visual Tracking
 *
 * \name Condensation
 * \author Daniel Pacheco de Queiroz
 * \author David Lunardi Flam
 * \version 1.0
 * \since 03/16/2010
 */
#include "Condensation.h"

Condensation::Condensation() {

}

Condensation::~Condensation() {

}

CvConDensation* Condensation::initCondensation (CvPoint2D32f initCoord, int nSample, int maxWidth, int maxHeight ){

	int DP = 2; // 4 state variables
	int MP = 2; // 2 measurement variables

	CvConDensation* conDens = cvCreateConDensation( DP, MP, nSample );
	CvMat* lowerBound;
	CvMat* upperBound;
	lowerBound = cvCreateMat(DP, 1, CV_32F);
	upperBound = cvCreateMat(DP, 1, CV_32F);

	// The bounds of the dimensions of the state vector, defines the quantity of noise
	// Sets the X bounds
	cvmSet( lowerBound, 0, 0, 0.0 );
	cvmSet( upperBound, 0, 0, maxWidth/1.5 ); // maxWidth/3

	// Sets the Y bounds
	cvmSet( lowerBound, 1, 0, 0.0 );
	cvmSet( upperBound, 1, 0, maxHeight/1.5 ); // maxHeight/3

	/*// Sets the X velocity bounds
	cvmSet( lowerBound, 2, 0, 0.0 );
	cvmSet( upperBound, 2, 0, 100.0 ); // Sets the spread of the particles

	// Sets the Y velocity bounds
	cvmSet( lowerBound, 3, 0, 0.0 );
	cvmSet( upperBound, 3, 0, 100.0 ); // 30.0*/
	// Sets the spread of the particles (if bigger, particles will be more dispersive and the area of search will increase,
	// possibly involving other marks, and changing the semantic)


	const float A[] = {1,0, 0,1};//, 0,0,0,0, 0,0,0,0}; // const float A[] = {1,0,1,0, 0,1,0,1, 0,0,1,0, 0,0,0,1};
	memcpy( conDens->DynamMatr, A, sizeof(A)); // Sets the Dynamic matrix

	cvConDensInitSampleSet(conDens, lowerBound, upperBound);

	CvRNG rng_state = cvRNG(0xffffffff);

	// Spreads the samples (or particles) over the area, with integer values (all initial velocities are 0)
	for(int i=0; i < nSample; i++){
		conDens->flSamples[i][0] = cvRandInt( &rng_state ) % maxWidth; //0 represent the widht (x coord)
		conDens->flSamples[i][1] = cvRandInt( &rng_state ) % maxHeight; //1 represent the height (y coord)
		//conDens->flSamples[i][2] = cvRandInt( &rng_state ) % (30); // Ou 0
		//conDens->flSamples[i][3] = cvRandInt( &rng_state ) % (30);
	}


	// Sets the initial state
	conDens->State[0] = initCoord.x;
	conDens->State[1] = initCoord.y;
	//conDens->State[2] = 0;
	//conDens->State[3] = 0;

	//cout << "initCondensation" << nSample << maxWidth << maxHeight << endl;

	return conDens;
}



void Condensation::refreshPOIsPosition(map<string, POI>& currentPOIs, vector<POI>& detectedPOIs, int maxCoordX, int maxCoordY, int &radius) {


	//--- For every initialized POI
	for (map<string, POI>::const_iterator it = currentPOIs.begin(); it != currentPOIs.end(); ++it) {

		POI POIToTrack = it->second;

		//cout << "Antes " << POIToTrack.getCondensation() << endl;

		if (POIToTrack.getCondensation() == NULL){
			POIToTrack.setCondensation( this->initCondensation(POIToTrack.getCoordinates2d(), 100, maxCoordX, maxCoordY) );

			cout << "Criou Condensation" << endl;
		}

		//cout << "Depois " << POIToTrack.getCondensation() << endl;
		CvConDensation* condensation =  POIToTrack.getCondensation();

		// Standard Deviation (used to determine the area of search)
		float stdDXcondens, stdDYcondens;

		CvPoint2D32f measuredPosition = POIToTrack.getCoordinates2d();
		//CvPoint2D32f measuredPosition = cvPoint2D32f(0.0,0.0);

		//Update Condensation and predict
		CvPoint2D32f predictedPosition = this->updateCondensation ( condensation, measuredPosition, &stdDXcondens, &stdDYcondens);

		/*cout << "XPred: " << predictedPosition.x << endl;
		cout << "YPred: " << predictedPosition.y << endl;
		cout << "XMea: " << measuredPosition.x << endl;
		cout << "YMea: " << measuredPosition.y << endl;*/

		POIToTrack.setPredictedPosition(predictedPosition);

		//CvPoint2D32f predictedVelocity = POIToTrack.getVelocityVector2d();
		//predictedVelocity.x = (int)predict->data.fl[2];
		//predictedVelocity.y = (int)predict->data.fl[3];

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


		//TODO: A area pode ser estimada pelo desvio padrão (o maior deles)
		double percentMaxDist = 0.05;
		double maxDist = Utils::cvPoint2d32fDist(origin, maxCoord) * percentMaxDist;
		maxDist = radius = stdDXcondens/2 + stdDYcondens/2; //radius;


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
		if (minDistIndex >= 0) { // Founded a valid point

			CvPoint2D32f measuredPosition = detectedPOIs[minDistIndex].getCoordinates2d();

			// New velocity calculated by Condensation
			//CvPoint2D32f newVelocity;
			//newVelocity.x = condensation->State[2];
			//newVelocity.y = condensation->State[3];
			//POIToTrack.setVelocityVector2d(newVelocity);

			//--- Refreshes POI position
			POIToTrack.setCoordinates2d(measuredPosition);

			//cout << "Semantica " << POIToTrack.getSemantic().toStdString() << endl;
			currentPOIs[POIToTrack.getSemantic().toStdString()] = POIToTrack;

			//--- Removes used detected POI
			std::swap(detectedPOIs[minDistIndex], detectedPOIs[detectedPOIs.size() - 1]);
			detectedPOIs.pop_back();

		//--- Inertia
		} else {

			//POIToTrack.setCoordinates2d(predictedPosition);
			//currentPOIs[POIToTrack.getSemantic().toStdString()] = POIToTrack;

		}
		//cout << "Depois " << currentPOIs[POIToTrack.getSemantic().toStdString()].getCondensation() << endl;
	}


}


CvPoint2D32f Condensation::updateCondensation ( CvConDensation* conDens, CvPoint2D32f Measurement, float * stdDX_ptr, float * stdDY_ptr){

	//cout << "INICIO updateCondensation" << endl;
	CvPoint2D32f prediction;

	float ProbX, ProbY, stdDevX, stdDevY , varianceX, varianceY;

	// Initialize the probabilities with 1 (total)
	ProbX=1; ProbY=1;

	// Make new vectors of samples for each coordinate separately
	//sampleStat* statSampleX = new sampleStat (conDens->SamplesNum);
	//sampleStat* statSampleY = new sampleStat (conDens->SamplesNum);

	// Sets the vectors of Samples (could change)
	/*for(int i = 0; i < conDens->SamplesNum; i++)
	{
		statSampleX->setValue(conDens->flSamples[i][0],i);
		statSampleY->setValue(conDens->flSamples[i][1],i);
	}*/

	// Calculate the variance of each coordinate
	varianceX = Utils::getVariance(conDens->flSamples, conDens->SamplesNum, 0);
	varianceY = Utils::getVariance(conDens->flSamples, conDens->SamplesNum, 1);

	// Calculate the standard deviation of each coordinate
	stdDevX = sqrt(varianceX);
	stdDevY = sqrt(varianceY);

	*stdDX_ptr = stdDevX;
	*stdDY_ptr = stdDevY;

	varianceX = 2 * varianceX;
	varianceY = 2 * varianceY;

	float xValue = 0;
	float yValue = 0;

	// For all particles, calculates the probability
	for(int i = 0; i < conDens->SamplesNum; i++)
	{

		xValue = Measurement.x - conDens->flSamples[i][0];
		ProbX = (float) exp( (-1 * xValue * xValue) / varianceX );

		yValue = Measurement.y - conDens->flSamples[i][1];
		ProbY = (float) exp( (-1  * yValue * yValue) / varianceY ) ;

		// Combine the probabilities of each coordinate (sum of probabilities).
		conDens->flConfidence[i] = ProbX*ProbY;
	}



	//cout << "INICIO cvConDensUpdateByTime" << endl;
	// Update the condensation structure
	cvConDensUpdateByTime(conDens);
	//cout << "FIM cvConDensUpdateByTime" << endl;

	prediction.x = conDens->State[0];
	prediction.y = conDens->State[1];

	//cout << "FIM updateCondensation" << endl;

	return prediction;
}

void Condensation::restartTracker(){


}
