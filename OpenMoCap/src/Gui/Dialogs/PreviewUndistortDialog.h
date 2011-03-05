#ifndef PREVIEWUNDISTORTDIALOG_H_
#define PREVIEWUNDISTORTDIALOG_H_

#include <iostream>
#include <QObject>

#include "../Widgets/ImageWidget.h"

using namespace std;

class PreviewUndistortDialog : public QDialog {

Q_OBJECT

public:

	/*!
	 * PreviewUndistortDialog constructor.
	 *
	 * @param parent The parent dialog.
	 * @param distortedImage The distorted calibration image.
	 * @param undistortedImage The undistorted calibration image.
	 */
	PreviewUndistortDialog(QWidget* parent = 0, IplImage* distortedImage = 0, IplImage* undistortedImage = 0);

	/*!
	 * PreviewUndistortDialog destructor.
	 */
	~PreviewUndistortDialog();

private:

	//! The distorted calibration image.
	ImageWidget* _distortedDisplay;

	//! Horizontal box layout of the original calibration image.
	QHBoxLayout* _originalImageLayout;

	//! Group box frame with the original calibration image.
	QGroupBox* _originalImageGroupBox;

	//! The undistorted calibration image.
	ImageWidget* _undistortedDisplay;

	//! Horizontal box layout of the corrected calibration image.
	QHBoxLayout* _correctedImageLayout;

	//! Group box frame with the corrected calibration image.
	QGroupBox* _correctedImageGroupBox;

	//! Horizontal box layout of the preview of undistorted calibration image dialog.
	QHBoxLayout* _mainLayout;
};

#endif /* PREVIEWUNDISTORTDIALOG_H_ */
