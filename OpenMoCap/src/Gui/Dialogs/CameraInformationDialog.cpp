/*!
 * Class that implements the camera information dialog.
 *
 * \name CameraInformationDialog
 * \author David Lunardi Flam
 * \version
 * \since 09/04/2008
 */
#include "CameraInformationDialog.h"

void CameraInformationDialog::refreshPreviewUndistortButton() {

	//---Same conditions apply for previewUndistort and calibrate just extrinsic parameters
	if (_cameraRef->getDistortionCoefficients() != NULL && (_calibrationImageNames->size() == 1)) {
		_previewUndistortButton->setEnabled(true);
		_calibrateExtrinsicPushButton->setEnabled(true);
	} else {
		_previewUndistortButton->setEnabled(false);
		_calibrateExtrinsicPushButton->setEnabled(false);
	}
}

void CameraInformationDialog::refreshCalibrateButton() {

	//--- If one or more images have been selected enable calibration button
	if (_calibrationImageNames->size() > 0) {
		_calibratePushButton->setEnabled(true);
	} else {
		_calibratePushButton->setDisabled(true);
	}
}

void CameraInformationDialog::selectCalibrationImages() {

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("Images (*.png *.ppm *.xpm *.jpg)"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setViewMode(QFileDialog::Detail);

	if (dialog.exec()) {
		*_calibrationImageNames = dialog.selectedFiles();
	}

	//--- Label with number of files is refreshed
	_labelImageNames->setText(QString("%1 file(s) selected.").arg(_calibrationImageNames->size()));

	refreshCalibrateButton();
	refreshPreviewUndistortButton();
}

void CameraInformationDialog::createFormCalibrationBox() {

	_labelCameraName = new QLabel(QString(_cameraRef->getName().c_str()));
	_labelCamera = new QLabel("Camera");

	_informationText = new QTextEdit();
	_informationText->setReadOnly(true);
	_informationText->setText(_cameraRef->getInfo());
	_labelInformationText = new QLabel("Information:");

	_calibrationImageNames = new QStringList();
	_labelImageNames = new QLabel(QString("%1 file(s) selected.").arg(_calibrationImageNames->size()));
	_selectImagesButton = new QPushButton("Select Image(s)");
	connect(_selectImagesButton, SIGNAL(clicked()), this, SLOT(selectCalibrationImages()));

	_formLayout = new QFormLayout();
	_formLayout->addRow(_labelCamera, _labelCameraName);
	_formLayout->addRow(_labelImageNames, _selectImagesButton);
	_formLayout->addRow(_labelInformationText, _informationText);

	_formCalibrationBox = new QGroupBox();
	_formCalibrationBox->setLayout(_formLayout);
}

void CameraInformationDialog::calibrateCamera() {

	_calibratorRef->setImageFileNames(*_calibrationImageNames);
	CalibrationResultEnum calibrationResult = _calibratorRef->calibrate(_cameraRef);

	_informationText->setText(calibrationResult.getString().c_str());
	_informationText->append(_cameraRef->getInfo());
	refreshPreviewUndistortButton();
}

void CameraInformationDialog::calibrateCameraExtrinsicParameters() {

	_calibratorRef->setImageFileNames(*_calibrationImageNames);
	CalibrationResultEnum calibrationResult = _calibratorRef->calibrateExtrinsicParameters(_cameraRef);

	_informationText->setText(calibrationResult.getString().c_str());
	_informationText->append(_cameraRef->getInfo());
	refreshPreviewUndistortButton();
}

void CameraInformationDialog::previewUndistort() {

	Image* images = _cameraRef->previewUndistort(_calibrationImageNames->at(0).toStdString());

	if (images[0].getIplImage() != NULL) {
		PreviewUndistortDialog previewUndistortDialog(this, images[0].getIplImage(), images[1].getIplImage());
		previewUndistortDialog.exec();
	}

	delete[] images;
}

void CameraInformationDialog::createControlButtonBox() {

	_okPushButton = new QPushButton("Ok");
	connect(_okPushButton, SIGNAL(clicked()), this, SLOT(accept()));

	_calibratePushButton = new QPushButton("Calibrate All Parameters");
	//---No Images have been selected yet; calibration not possible
	_calibratePushButton->setEnabled(false);
	_calibratePushButton->setToolTip("Select one or more images.");
	connect(_calibratePushButton, SIGNAL(clicked()), this, SLOT(calibrateCamera()));

	_calibrateExtrinsicPushButton = new QPushButton("Calibrate Extrinsic Parameters");
	_calibrateExtrinsicPushButton->setEnabled(false);
	_calibrateExtrinsicPushButton->setToolTip("Select just one image.");
	connect(_calibrateExtrinsicPushButton, SIGNAL(clicked()), this, SLOT(calibrateCameraExtrinsicParameters()));

	_previewUndistortButton = new QPushButton("Preview Undistort");
	_previewUndistortButton->setToolTip("Select just one image.");
	_previewUndistortButton->setEnabled(false);
	connect(_previewUndistortButton, SIGNAL(clicked()), this, SLOT(previewUndistort()));

	_vBoxLayout = new QVBoxLayout();
	_vBoxLayout->addWidget(_calibratePushButton);
	_vBoxLayout->addWidget(_calibrateExtrinsicPushButton);
	_vBoxLayout->addWidget(_previewUndistortButton);
	_vBoxLayout->addWidget(_okPushButton);

	_controlButtonBox = new QGroupBox();
	_controlButtonBox->setLayout(_vBoxLayout);
}

void CameraInformationDialog::createMainLayout() {

	_mainHBoxLayout = new QHBoxLayout(this);
	_mainHBoxLayout->addWidget(_formCalibrationBox);
	_mainHBoxLayout->addWidget(_controlButtonBox);
	setLayout(_mainHBoxLayout);
}

CameraInformationDialog::CameraInformationDialog(QWidget *parent, AbstractCamera* camera,
		AbstractCalibrator* calibrator) :
	QDialog(parent), _cameraRef(camera), _calibratorRef(calibrator) {

	setWindowTitle("Camera Information and Calibration");

	createFormCalibrationBox();
	createControlButtonBox();
	createMainLayout();
}

CameraInformationDialog::~CameraInformationDialog() {

}

#include "CameraInformationDialog.moc"
