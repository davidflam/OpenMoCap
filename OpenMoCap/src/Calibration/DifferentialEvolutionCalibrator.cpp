/*
 * DifferentialEvolutionCalibrator.cpp
 *
 *  Created on: 26/03/2009
 *      Author: David Flam
 *      		Daniel Pacheco
 */

#include "DifferentialEvolutionCalibrator.h"

DifferentialEvolutionCalibrator::DifferentialEvolutionCalibrator(vector<AbstractCamera*>* camerasRef,
		double maxDepth, double maxRotationInRadians, double minFocalLengthInPixels,
		double maxFocalLengthInPixels) :
	_camerasRef(camerasRef), _maxDepth(maxDepth), _maxRotationInRadians(maxRotationInRadians),
			_minFocalLengthInPixels(minFocalLengthInPixels), _maxFocalLengthInPixels(maxFocalLengthInPixels) {

	_width = _camerasRef->at(0)->getWidth();
	_height = _camerasRef->at(0)->getHeight();
	_randomNumberGenerator = cvRNG((unsigned) time(0));

}

vector<POI> DifferentialEvolutionCalibrator::calibrate(int numIndividuals, int generations,
		double differentialVariation, double recombinationConstant, double& cost,
		vector<map<string, POI> >& calibrationPoints, QProgressBar* progressBarRef) {

	_calibrationPoints = calibrationPoints;
	//  TODO (david) testing purposes only!!!
	//	getMocPOIs(_calibrationPoints);

	//--- Rotation and Translation Vector for each camera plus focus
	_numVariables = (3 + 3) * _camerasRef->size() + 1;
	_numPoints = _calibrationPoints[0].size();

	logDEBUG("D.E Calibration #Individuals %d #Generations %d", numIndividuals, generations);
	for (unsigned int i = 0; i < _calibrationPoints.size(); ++i) {
		logDEBUG("camera %d", i);
		for (map<string, POI>::iterator it = _calibrationPoints[i].begin(); it != _calibrationPoints[i].end(); it++) {
			logDEBUG(it->second.getInfo2d().toStdString().c_str());
		}
	}
	
	for (unsigned int i = 0; i < _calibrationPoints.size(); i++) {
		_points.push_back(cvCreateMat(1, _numPoints, CV_32FC2));
	}
	AbstractReconstructor::computePointArrays(_calibrationPoints, _points, _width, _height);
	InitializeIndividuals(numIndividuals);

	for (int generation = 0; generation < generations; generation++) {

		vector<Individual> newGeneration;
		vector<int> individualsToBeKilled;
		progressBarRef->setValue(generation);

		for (int i = 0; i < numIndividuals; i++) {

			//--- Select 3 different individuals
			int i1 = i;
			while (i1 == i) {
				i1 = cvRandReal(&_randomNumberGenerator) * (numIndividuals - 1);
			}

			int i2 = i;
			while (i2 == i || i2 == i1) {
				i2 = cvRandReal(&_randomNumberGenerator) * (numIndividuals - 1);
			}
			int i3 = i;
			while (i3 == i || i3 == i1 || i3 == i2) {
				i3 = cvRandReal(&_randomNumberGenerator) * (numIndividuals - 1);
			}

			int variableIndex = cvRandReal(&_randomNumberGenerator) * (_numVariables);

			Individual child(_numVariables);

			for (int v = 0; v < _numVariables; v++) {

				//--- Recombination with differential variation
				if (cvRandReal(&_randomNumberGenerator) < recombinationConstant || v == variableIndex) {

					child.getVariables()->data.fl[v] = _individuals[i3].getVariables()->data.fl[v]
							+ differentialVariation * (_individuals[i1].getVariables()->data.fl[v]
									- _individuals[i2].getVariables()->data.fl[v]);
				} else {
					child.getVariables()->data.fl[v] = _individuals[i].getVariables()->data.fl[v];
				}
			}

			//--- Minimization; if child costs less it will proceed to next generation
			if (isBetter(child, _individuals[i])) {
				newGeneration.push_back(child);
				individualsToBeKilled.push_back(i);
			} else {
				newGeneration.push_back(_individuals[i]);
				child.releaseMatrices();
			}
		}

		//--- Kill unselected individuals and restore mem
		for (unsigned int k = 0; k < individualsToBeKilled.size(); k++) {
			_individuals[individualsToBeKilled[k]].releaseMatrices();
		}
		_individuals = newGeneration;

	}

	cost = evaluateIndividual(_individuals[0]);
	vector<POI> POIs3d = get3dPOIsFromIndividual(_individuals[0], _calibrationPoints[0]);
	fillCameraParameters(_individuals[0], _camerasRef);

	return POIs3d;
}

DifferentialEvolutionCalibrator::~DifferentialEvolutionCalibrator() {

	//--- Release point arrays
	for (unsigned int i = 0; i < _calibrationPoints.size(); i++) {
		CvMat* points = _points[i];
		cvReleaseMat(&points);
	}
}

void DifferentialEvolutionCalibrator::getMocPOIs(vector<map<string, POI> >& calibrationPoints) {

	calibrationPoints.clear();
	ifstream pointsFile;
	pointsFile.open("Resources/reconstruction/points.txt");

	_numPoints = 0;

	if (pointsFile.is_open()) {

		string token;
		pointsFile >> token;
		_numPoints = atoi(token.c_str());

		int oldIndex = 0;
		int index = 0;
		int i = 0;
		map<string, POI> poisTemp;
		while (pointsFile >> token) {

			float x = atoi(token.c_str());

			pointsFile >> token;
			float y = atoi(token.c_str());

			POI poi(x, y);
			pointsFile >> token;
			poi.setSemantic(QString(token.c_str()));

			poisTemp[token] = poi;
			index = i / _numPoints;
			if (index > oldIndex) {
				calibrationPoints.push_back(poisTemp);
				oldIndex = index;
			}

			i++;
		}
		calibrationPoints.push_back(poisTemp);
	} else {
		logERROR("Points file not found!");
	}

}

void DifferentialEvolutionCalibrator::InitializeIndividuals(int numIndividuals) {

	if (_individuals.size() > 0) {
		clearIndividuals();
	}

	for (int i = 0; i < numIndividuals; i++) {

		Individual individual(_numVariables);

		for (int v = 0; v < _numVariables; v++) {

			//--- Double inside [0..1]
			double rand = cvRandReal(&_randomNumberGenerator);

			if (v < 3) {
				//--- Translation = 0 for first camera!
				rand = 0;
			} else if (v < (_numVariables - 1) / 2) { // First half, Translations values

				double randSign = cvRandReal(&_randomNumberGenerator);

				if (randSign > 0.5) {
					rand = -rand;
				}
			} else if (v < ((_numVariables - 1) / 2) + 3) { // Second half, Rotations values
				//--- Rotation = 0 for first camera!
				rand = 0;
			} else {
				rand = rand * _maxRotationInRadians;
			}

			individual.getVariables()->data.fl[v] = rand;

		}

		//--- Focus
		double rand = cvRandReal(&_randomNumberGenerator);
		individual.getVariables()->data.fl[_numVariables - 1] = (rand * (_maxFocalLengthInPixels
				- _minFocalLengthInPixels)) + _minFocalLengthInPixels;
		//TODO Fixed value focus
		individual.getVariables()->data.fl[_numVariables - 1] = 500;

		_individuals.push_back(individual);

	}

}

bool DifferentialEvolutionCalibrator::isBetter(Individual& child, Individual& father) {

	bool better = true;

	//--- Constraints
	for (int v = 0; v < _numVariables - 1; v++) {

		float value = child.getVariables()->data.fl[v];

		if (v < 3) {
			//--- Translation = 0 for first camera!
			if (value != 0) {
				better = false;
			}
		} else if (v < (_numVariables - 1) / 2) { // First half, Translations values
			if (value > 1.0f || value < -1.0f) {
				better = false;
			}
		} else if (v < ((_numVariables - 1) / 2) + 3) { // Second half, Rotations values
			//--- Rotation = 0 for first camera!
			if (value != 0) {
				better = false;
			}
		} else {
			if (value > _maxRotationInRadians || value < 0.0f) {
				better = false;
			}
		}
	}

	// Focus
	float value = child.getVariables()->data.fl[_numVariables - 1];
	if (value > _maxFocalLengthInPixels || value < _minFocalLengthInPixels) {
		better = false;
	}

	//--- Function Evaluation
	if (evaluateIndividual(child) > evaluateIndividual(father)) {
		better = false;
	}

	return better;

}

void DifferentialEvolutionCalibrator::clearIndividuals() {

	for (unsigned int i = 0; i < _individuals.size(); i++) {
		_individuals[i].releaseMatrices();
	}
	_individuals.clear();
}

double DifferentialEvolutionCalibrator::evaluateIndividual(Individual& individual) {

	//--- Variables order (2 cameras example):
	//--- Tx, Ty, Tz, Tx', Ty', Tz', a, b, g, a', b', g', f
	//--- 0   1   2   3    4    5    6  7  8  9   10  11  12

	double cost = 0;

	//--- K ; Intrinsic parameter matrix
	CvMat* kMatrix = AbstractCamera::computeKMatrix(individual.getVariables()->data.fl[_numVariables - 1], (_width / 2)
			- 1, (_height / 2) - 1);

	int numCam = _calibrationPoints.size();
	vector<CvMat*> rtMatrix(numCam);
	vector<CvMat*> cameraMatrix(numCam);

	for (int i = 0; i < numCam; i++) {
		//--- [R|T]
		rtMatrix[i] = AbstractCamera::computeRtMatrix(individual.getVariables()->data.fl[(numCam * 3) + (i * 3)],
				individual.getVariables()->data.fl[(numCam * 3) + (i * 3) + 1],
				individual.getVariables()->data.fl[(numCam * 3) + (i * 3) + 2], individual.getVariables()->data.fl[i
						* 3], individual.getVariables()->data.fl[(i * 3) + 1], individual.getVariables()->data.fl[(i
						* 3) + 2]);

		//--- M ; First camera projection matrix
		cameraMatrix[i] = cvCreateMat(3, 4, CV_32FC1);
		cvMatMul(kMatrix, rtMatrix[i], cameraMatrix[i]);

		cvReleaseMat(&rtMatrix[i]);
	}
	cvReleaseMat(&kMatrix);

	for (int i = 0; i < _numPoints; i++) {

		vector<CvPoint2D32f> points(numCam);

		for (int cam = 0; cam < numCam; cam++) {
			points[cam] = cvPoint2D32f(_points[cam]->data.fl[i * 2], _points[cam]->data.fl[i * 2 + 1]);
		}

		CvMat* estimated3dHomogeneousPoint = AbstractReconstructor::compute3dHomogeneousCoordinates(cameraMatrix,
				points);

		//--- Back project and calculate error
		CvMat* estimated2dHomogeneousPoint;
		for (int cam = 0; cam < numCam; cam++) {
			estimated2dHomogeneousPoint = cvCreateMat(3, 1, CV_32FC1);

			cvMatMul(cameraMatrix[cam], estimated3dHomogeneousPoint, estimated2dHomogeneousPoint);

			cvConvertScale(estimated2dHomogeneousPoint, estimated2dHomogeneousPoint, 1.0f / cvmGet(
					estimated2dHomogeneousPoint, 2, 0));

			CvPoint2D32f estimatedP = cvPoint2D32f(cvmGet(estimated2dHomogeneousPoint, 0, 0), cvmGet(
					estimated2dHomogeneousPoint, 1, 0));

			cvReleaseMat(&estimated2dHomogeneousPoint);

			cost += Utils::cvPoint2d32fDist(points[cam], estimatedP);

			//--- Z coordinate must be positive, structure in front of the camera
			if (cvmGet(estimated3dHomogeneousPoint, 2, 0) < 0.0f) {
				cost = FLT_MAX;
			}
		}

		cvReleaseMat(&estimated3dHomogeneousPoint);
	}

	for (int i = 0; i < numCam; i++) {
		cvReleaseMat(&cameraMatrix[i]);
	}

	return cost;

}

void DifferentialEvolutionCalibrator::fillCameraParameters(Individual& individual, vector<AbstractCamera*>* camerasRef) {

	//--- Variables order:
	//--- Tx, Ty, Tz, Tx', Ty', Tz', a, b, g, a', b', g', f
	//--- 0   1   2   3    4    5    6  7  8  9   10  11  12

	int numCam = _camerasRef->size();

	CvMat* kMatrix = AbstractCamera::computeKMatrix(individual.getVariables()->data.fl[_numVariables - 1], (_width / 2)
			- 1, (_height / 2) - 1);
	CvPoint3D32f cameraTranslation;
	CvPoint3D32f cameraRotation;

	for (int i = 0; i < numCam; i++) {
		cameraTranslation = cvPoint3D32f(individual.getVariables()->data.fl[(i * 3)],
				individual.getVariables()->data.fl[(i * 3) + 1], individual.getVariables()->data.fl[(i * 3) + 2]);

		cameraRotation = cvPoint3D32f(individual.getVariables()->data.fl[(numCam * 3) + (i * 3)],
				individual.getVariables()->data.fl[(numCam * 3) + (i * 3) + 1],
				individual.getVariables()->data.fl[(numCam * 3) + (i * 3) + 2]);

		camerasRef->at(i)->setTranslation(cameraTranslation);
		camerasRef->at(i)->setRotation(cameraRotation);
		camerasRef->at(i)->setIntrinsicParams(kMatrix);
	}

}

vector<POI> DifferentialEvolutionCalibrator::get3dPOIsFromIndividual(Individual& individual,
		map<string, POI> firstCameraPOIs) {

	vector<POI> POIs3d;

	//--- K ; Intrinsic parameter matrix
	CvMat* kMatrix = AbstractCamera::computeKMatrix(individual.getVariables()->data.fl[_numVariables - 1], (_width / 2)
			- 1, (_height / 2) - 1);

	int numCam = _calibrationPoints.size();
	vector<CvMat*> rtMatrix(numCam);
	vector<CvMat*> cameraMatrix(numCam);

	for (int i = 0; i < numCam; i++) {
		//--- [R|T]
		rtMatrix[i] = AbstractCamera::computeRtMatrix(individual.getVariables()->data.fl[(numCam * 3) + (i * 3)],
				individual.getVariables()->data.fl[(numCam * 3) + (i * 3) + 1],
				individual.getVariables()->data.fl[(numCam * 3) + (i * 3) + 2], individual.getVariables()->data.fl[i
						* 3], individual.getVariables()->data.fl[(i * 3) + 1], individual.getVariables()->data.fl[(i
						* 3) + 2]);

		//--- M ; First camera projection matrix
		cameraMatrix[i] = cvCreateMat(3, 4, CV_32FC1);
		cvMatMul(kMatrix, rtMatrix[i], cameraMatrix[i]);

		cvReleaseMat(&rtMatrix[i]);
	}
	cvReleaseMat(&kMatrix);

	int i = 0;
	for (map<string, POI>::const_iterator it = firstCameraPOIs.begin(); it != firstCameraPOIs.end(); ++it) {

		POI firstCameraPOI = it->second;

		vector<CvPoint2D32f> points(numCam);

		for (int cam = 0; cam < numCam; cam++) {
			points[cam] = cvPoint2D32f(_points[cam]->data.fl[i * 2], _points[cam]->data.fl[i * 2 + 1]);
		}

		CvMat* estimated3dHomogeneousPoint = AbstractReconstructor::compute3dHomogeneousCoordinates(cameraMatrix,
				points);

		firstCameraPOI.setCoordinates3d(cvPoint3D32f(cvmGet(estimated3dHomogeneousPoint, 0, 0), cvmGet(
				estimated3dHomogeneousPoint, 1, 0), cvmGet(estimated3dHomogeneousPoint, 2, 0)));

		POIs3d.push_back(firstCameraPOI);

		i++;

	}

	for (int i = 0; i < numCam; i++) {
		cvReleaseMat(&cameraMatrix[i]);
	}

	return POIs3d;
}
