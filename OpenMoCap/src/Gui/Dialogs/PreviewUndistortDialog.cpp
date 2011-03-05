/*!
 * Class that implements the preview of undistorted calibration image dialog.
 *
 * \name PreviewUndistortDialog
 * \author David Lunardi Flam
 * \version
 * \since 09/29/2008
 */
#include "PreviewUndistortDialog.h"

PreviewUndistortDialog::PreviewUndistortDialog(QWidget* parent, IplImage* distortedImage, IplImage* undistortedImage) : QDialog(parent) {

	int width = distortedImage->width;
	int height = distortedImage->height;

	_distortedDisplay = new ImageWidget(this, width, height);
	_distortedDisplay->refreshImage(distortedImage);

	_originalImageLayout = new QHBoxLayout();
	_originalImageLayout->addWidget(_distortedDisplay);

	_originalImageGroupBox = new QGroupBox("Original Image");
	_originalImageGroupBox->setLayout(_originalImageLayout);

	_undistortedDisplay = new ImageWidget(this, width, height);
	_undistortedDisplay->refreshImage(undistortedImage);

	_correctedImageLayout = new QHBoxLayout();
	_correctedImageLayout->addWidget(_undistortedDisplay);

	_correctedImageGroupBox = new QGroupBox("Corrected Image");
	_correctedImageGroupBox->setLayout(_correctedImageLayout);

	_mainLayout = new QHBoxLayout();
	_mainLayout->addWidget(_originalImageGroupBox);
	_mainLayout->addWidget(_correctedImageGroupBox);

	setLayout(_mainLayout);
	setWindowTitle("Preview Undistort");

}

PreviewUndistortDialog::~PreviewUndistortDialog() {

}

#include "PreviewUndistortDialog.moc"
