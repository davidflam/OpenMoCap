#ifndef CAMERAINFORMATIONDIALOG_H_
#define CAMERAINFORMATIONDIALOG_H_

#include <iostream>
#include <QObject>

#include "../../Calibration/ZhangCalibrator.h"
#include "../../Entities/AbstractCamera.h"
#include "../Widgets/CameraComboBox.h"
#include "PreviewUndistortDialog.h"

using namespace std;

class CameraInformationDialog: public QDialog {

Q_OBJECT

public:

	/*!
	 * CameraInformationDialog constructor.
	 *
	 * @param parent The parent dialog.
	 * @param camera The camera that will be its information displayed.
	 * @param calibrator The calibrator to be used.
	 */
	CameraInformationDialog(QWidget *parent = 0, AbstractCamera* camera = NULL, AbstractCalibrator* calibrator = NULL);

	/*!
	 * CameraInformationDialog destructor.
	 */
	~CameraInformationDialog();

public slots:

	/*!
	 * Selects the calibration images.
	 */
	void selectCalibrationImages();

	/*!
	 * Displays the camera calibration result.
	 */
	void calibrateCamera();

	/*!
	 * Calibrates only extrinsic parameters
	 */
	void calibrateCameraExtrinsicParameters();

	/*!
	 * Displays the undistorted calibration image.
	 */
	void previewUndistort();

private:

	//--- Shared Core Variables; must not be deleted inside destructor method.

	//! Camera reference.
	AbstractCamera* _cameraRef;

	//! Calibrator reference.
	AbstractCalibrator* _calibratorRef;

	//! Group box frame of calibration information.
	QGroupBox* _formCalibrationBox;

	//! Form layout.
	QFormLayout* _formLayout;

	//! Camera label.
	QLabel* _labelCamera;

	//! Camera name label.
	QLabel* _labelCameraName;

	//! Information text.
	QTextEdit* _informationText;

	//! Information text label.
	QLabel* _labelInformationText;

	//! List with the names of the calibration images.
	QStringList* _calibrationImageNames;

	//! Image names label.
	QLabel* _labelImageNames;

	//! Select images button.
	QPushButton* _selectImagesButton;

	/*!
	 * Creates the group box frame with the calibration information.
	 */
	void createFormCalibrationBox();

	//! Group box frame with control buttons.
	QGroupBox* _controlButtonBox;

	//! Vertical box layout of the control buttons.
	QVBoxLayout* _vBoxLayout;

	//! OK button.
	QPushButton* _okPushButton;

	//! Calibrate button.
	QPushButton* _calibratePushButton;

	//! Calibrate extrinsic button.
	QPushButton* _calibrateExtrinsicPushButton;

	/*!
	 * Refreshes calibrate button.
	 */
	void refreshCalibrateButton();

	//! Preview undistorted calibration image button.
	QPushButton* _previewUndistortButton;

	/*!
	 * Refreshes the preview undistort button.
	 */
	void refreshPreviewUndistortButton();

	/*!
	 * Creates the group box frame with the control buttons.
	 */
	void createControlButtonBox();

	//! Horizontal box layout of the camera information dialog.
	QHBoxLayout* _mainHBoxLayout;

	/*!
	 * Creates the main layout of camera information dialog.
	 */
	void createMainLayout();

};

#endif /* CAMERAINFORMATIONDIALOG_H_ */
