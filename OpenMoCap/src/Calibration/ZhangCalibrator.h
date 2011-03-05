#ifndef ZHANGCALIBRATOR_H_
#define ZHANGCALIBRATOR_H_

#include "../Entities/AbstractCamera.h"
#include "../Utils/Image.h"
#include "AbstractCalibrator.h"

using namespace std;

//! Number of internal corners per rows in the calibration plane.
#define NUM_INT_CORNERS_ROW 5
//! Number of internal corners per columns in the calibration plane.
#define NUM_INT_CORNERS_COL 5
//! Total number of internal corners in the calibration plane.
#define NUM_INT_CORNERS (NUM_INT_CORNERS_ROW * NUM_INT_CORNERS_COL)
//! Width of the squares contained in the calibration plane.
#define PATTERN_SQ_WIDTH 5
//! Number of distortion coefficients.
#define NUM_DIST_COEFFS 4
//! Number of elements of a translation vector.
#define NUM_ELE_TRANS_VECTOR 3

class ZhangCalibrator : public AbstractCalibrator {

public:

	/*!
	 * ZhangCalibrator constructor.
	 */
	ZhangCalibrator();

	/*!
	 * ZhangCalibrator destructor. If there are valid images, the arrays were initialized and must be released.
	 */
	~ZhangCalibrator();

	/*!
	 * Calibrates all camera parameters (extrinsic + intrinsic)
	 */
	CalibrationResultEnum calibrate(AbstractCamera *camera);

	/*!
	 * Calibrates just camera's extrinsic parameters
	 */
	CalibrationResultEnum calibrateExtrinsicParameters(AbstractCamera *camera);

private:

	//! The calibration pattern size.
	static CvSize _patternSize;

	//! The calibration pattern resolution.
	CvSize _patternResolution;

	//! Array containing the corners coordinates of each calibration image.
	CvPoint3D32f* _objectPoints;

	//! Array containing the number of corners of each calibration image.
	int* _pointCounts;

	//! Array containing the corners of each calibration image.
	CvPoint2D32f* _corners;

	//! Vector containing the names of the calibration images.
	vector<string> _validImages;

	/*!
	 * Verifies if the calibration images are valid.
	 */
	void verifyImages();

	/*!
	 * Initializes the array containing the corners coordinates of each calibration image.
	 */
	void initializeObjectPoints();

	/*!
	 * Initializes the array containing the number of corners of each calibration image.
	 */
	void initializePointCounts();

	/*!
	 * Initializes the corners of each calibration images.
	 */
	void initializeCorners();

	/*!
	 * Finds the corners of the calibration image.
	 *
	 * @param image The calibration image.
	 *
	 * @return If the corners were found this function returns "true", otherwise, "false".
	 */
	bool findCorners(Image& image);

	/*!
	 * Fills the array containing the calibration image corners.
	 *
	 * @param image The calibration image.
	 * @param corners The corners of the calibration image.
	 */
	void fillCorners(Image& image, CvPoint2D32f* corners);

};

#endif /* ZHANGCALIBRATOR_H_ */
