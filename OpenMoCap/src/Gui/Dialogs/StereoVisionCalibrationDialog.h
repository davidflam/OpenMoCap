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
#include "../../Controllers/CaptureController.h"

using namespace std;

static const int MSEC_BETWEEN_CALIB_IMAGES = 5000;
class CaptureController;

class StereoVisionCalibrationDialog: public QDialog {

Q_OBJECT
public:
    StereoVisionCalibrationDialog(CaptureController *captureController);
    bool foundValidCornersForStereoCalibration();
private slots:
    void calibrate();
    void changeParameter(double value);
    void beginRecordingImages();
    void stopRecordingImages();
    void recordCalibrationImage();
private:
    CaptureController *_captureControllerRef;
    vector<AbstractCamera*> *_camerasRef;
    ThreeDWidget *_modelVisualizationRef;
    vector<POI> _calibrationPoints;
    QVBoxLayout *_mainLayout;
    QLabel *_numberAvailablePointsLabel;
    QPushButton *_beginRecordingPointsButton;
    QPushButton *_stopRecordingPointsButton;
    void createCalibrationImagesBox();
    QDoubleSpinBox *_numIntCornersRowSpinBox;
    double _numIntCornersRow;
    QDoubleSpinBox *_numIntCornersColSpinBox;
    double _numIntCornersCol;
    QDoubleSpinBox *_calibrationPatternWidthSpinBox;
    double _calibrationPatternWidth;
    QDoubleSpinBox *_calibrationPatternHeightSpinBox;
    double _calibrationPatternHeight;
    void createAlgorithmParametersBox();
    QDoubleSpinBox *_maxDepthSpinBox;
    double _maxDepth;
    QDoubleSpinBox *_rotationMaxSpinBox;
    double _rotationMaxInDegrees;
    QDoubleSpinBox *_focalLengthMaxSpinBox;
    double _focalLengthMax;
    QDoubleSpinBox *_focalLengthMinSpinBox;
    double _focalLengthMin;
    void createCameraParametersBox();
    QPushButton *_calibrateButton;
    QProgressBar *_progresBar;
    void createCalibrationBox();
    QTextEdit *_resultsText;
    void refreshAvailableCalibrationPointsLabel();
    QTimer* _recordPointsTimer;

};

#endif /* STEREOVISIONCALIBRATIONDIALOG_H_ */
