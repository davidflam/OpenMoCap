/*
 * StereoVisionCalibrationDialog.h
 *
 *  Created on: 30/03/2009
 *      Author: David
 */

#ifndef STEREOVISIONCALIBRATIONDIALOG_H_
#define STEREOVISIONCALIBRATIONDIALOG_H_

#include <iostream>
#include <QObject>
#include <QComboBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QProgressBar>
#include <vector>

#include "../Widgets/ThreeDWidget.h"
#include "../../Calibration/DifferentialEvolutionCalibrator.h"
#include "../../Calibration/DifferentialEvolutionCalibrator.h"

using namespace std;

class StereoVisionCalibrationDialog: public QDialog {

Q_OBJECT

public:
	StereoVisionCalibrationDialog(vector<AbstractCamera*>* camerasRef, ThreeDWidget* modelVisualization);
	/*!
	 * Checks if there is only one POI with semantic in each camera's view and if it has the same semantic across every video device
	 */
	static bool areCamerasStateValidForCalibrationPointRecording(vector<AbstractCamera*>* camerasRef);

private slots:
	void calibrate();
	void changeParameter(double value);
	void beginRecordingPoints();
	void stopRecordingPoints();
	void recordCalibrationPoint();

private:

	vector<AbstractCamera*>* _camerasRef;
	ThreeDWidget* _modelVisualizationRef;
	vector<map<string, POI> > _calibrationPoints;

	QVBoxLayout* _mainLayout;

	QLabel* _numberAvailablePointsLabel;
	QPushButton* _beginRecordingPointsButton;
	QPushButton* _stopRecordingPointsButton;

	void createCalibrationPointsBox();

	QDoubleSpinBox* _individualsSpinBox;
	double _individuals;

	QDoubleSpinBox* _generationsSpinBox;
	double _generations;

	QDoubleSpinBox* _differentialVariationSpinBox;
	double _differentialVariation;

	QDoubleSpinBox* _recombinationConstantSpinBox;
	double _recombinationConstant;

	void createAlgorithmParametersBox();

	QDoubleSpinBox* _maxDepthSpinBox;
	double _maxDepth;

	QDoubleSpinBox* _rotationMaxSpinBox;
	double _rotationMaxInDegrees;

	QDoubleSpinBox* _focalLengthMaxSpinBox;
	double _focalLengthMax;

	QDoubleSpinBox* _focalLengthMinSpinBox;
	double _focalLengthMin;

	void createCameraParametersBox();

	QPushButton* _calibrateButton;
	QProgressBar* _progresBar;

	void createCalibrationBox();

	QTextEdit* _resultsText;
	int _numCalibrationPoints;

	void refreshAvailableCalibrationPointsLabel();

	//--- Timer to capture points for calibration
	QTimer* _recordPointsTimer;

};

#endif /* STEREOVISIONCALIBRATIONDIALOG_H_ */
