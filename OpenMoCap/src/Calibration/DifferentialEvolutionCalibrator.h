/*
 * DifferentialEvolutionCalibrator.h
 *
 *  Created on: 26/03/2009
 *      Author: Daniel Pacheco
 *      		David Flam
 */

#ifndef DIFFERENTIALEVOLUTIONCALIBRATOR_H_
#define DIFFERENTIALEVOLUTIONCALIBRATOR_H_

#include "Individual.h"
#include "../Utils/Utils.h"
#include "../Utils/Timer.h"
#include "../Entities/POI.h"
#include "../Entities/AbstractCamera.h"
#include "../Reconstruction/AbstractReconstructor.h"
#include "../Utils/Log.h"

#include <ctime>
#include <fstream>
#include <map>
#include <vector>
#include <float.h>
#include <QProgressBar>

using namespace std;

class DifferentialEvolutionCalibrator {

public:
	DifferentialEvolutionCalibrator(vector<AbstractCamera*>* camerasRef, double maxDepth,
			double maxRotationInRadians, double minFocalLengthInPixels, double maxFocalLengthInPixels);
	~DifferentialEvolutionCalibrator();
	void getMocPOIs(vector<map<string, POI> >& pois);
	vector<POI> calibrate(int numIndividuals, int generations, double differentialVariation,
			double recombinationConstant, double& cost, vector<map<string, POI> >& calibrationPoints,
			QProgressBar* progressBarRef);

private:
	void InitializeIndividuals(int numIndividuals);
	void clearIndividuals();
	double evaluateIndividual(Individual& individual);
	bool isBetter(Individual& child, Individual& father);
	vector<POI> get3dPOIsFromIndividual(Individual& individual, map<string, POI> firstCameraPOIs);
	void fillCameraParameters(Individual& individual, vector<AbstractCamera*>* camerasRef);

	vector<Individual> _individuals;
	int _numVariables;
	vector<map<string, POI> > _calibrationPoints;

	vector<CvMat*> _points;
	int _numPoints;
	int _width;
	int _height;

	vector<AbstractCamera*>* _camerasRef; // Vector of cameras

	/*!
	 * Camera Parameters Boundaries
	 */

	double _maxDepth;
	double _maxRotationInRadians;
	double _minFocalLengthInPixels;
	double _maxFocalLengthInPixels;

	CvRNG _randomNumberGenerator;

};

#endif /* DIFFERENTIALEVOLUTIONCALIBRATOR_H_ */
