/*!
 * Abstract class for camera calibrators.
 *
 * \name AbstractCalibrator
 * \author David Lunardi Flam
 * \version
 * \since 10/01/2008
 */
#ifndef ABSTRACTCALIBRATOR_H_
#define ABSTRACTCALIBRATOR_H_

#include <QStringList>

#include "../Entities/AbstractCamera.h"
#include "../Enums/CalibrationResultEnum.h"

using namespace std;

class AbstractCalibrator {

public:

	/*!
	 * Sets the file name list of calibration images.
	 *
	 * @param imageFileNames File name list of images.
	 */
	void setImageFileNames(QStringList imageFileNames) {
		_imageFileNames = imageFileNames;
	}

	/*!
	 * Calibrate intrinsic and extrinsic camera parameters.
	 *
	 * @param camera Camera to be calibrated.
	 *
	 * @return The enumeration of calibration results.
	 */
	virtual CalibrationResultEnum calibrate(AbstractCamera *camera) = 0;

	/*! Calibrate just the rotation matrix and translation vector.
	 *
	 * @param camera Camera to be calibrated.
	 *
	 * @return The enumeration of calibration results.
	 */
	virtual CalibrationResultEnum calibrateExtrinsicParameters(AbstractCamera *camera) = 0;

protected:
	//! File name list of calibration images.
	QStringList _imageFileNames;
};

#endif /* ABSTRACTCALIBRATOR_H_ */
