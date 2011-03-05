#ifndef BLOBEXTRACTOR_H_
#define BLOBEXTRACTOR_H_

#include <algorithm>

#include "../Utils/Timer.h"
#include "../Utils/Image.h"
#include "AbstractPOIFinder.h"
#include "Blob.h"

#include "../Utils/Debug.h"

#define BACKGROUND_LABEL 0

using namespace std;

class BlobExtractor: public AbstractPOIFinder {

public:

	/*!
	 * BlobExtractor constructor.
	 */
	BlobExtractor();

	/*!
	 * BlobExtractor destructor.
	 */
	~BlobExtractor();

	vector<POI> getPOIsInImage(IplImage *image);

private:

	/*!
	 * Creates the blobs.
	 *
	 * @param blobs A vector containing the blobs.
	 * @param nBlobs Number of blobs to be created.
	 */
	void createBlobs(vector<Blob>& blobs, const int nBlobs);

	/*!
	 * Fills the blobs with their labels.
	 *
	 * @param blobs The vector that contains the blobs.
	 * @param labelCorrespondence The label correspondence.
	 * @param labels The blobs labels.
	 */
	void fillBlobs(vector<Blob>& blobs, const vector<int>& labelCorrespondence, Image& labels);

	/*!
	 * Finds the blobs in an image
	 *
	 * @param image The image to be used.
	 *
	 * @return
	 */
	vector<Blob> findBlobs(Image& image);

	/*!
	 * Removes the blobs outside the area limit.
	 *
	 * @param blobs the vector that contains the blobs.
	 * @param minNumPixels Minimum number of pixels.
	 * @param maxNumPixels Maximum number of pixels.
	 */
	void removeBlobsOutsideAreaLimit(vector<Blob>& blobs, const int minNumPixels, const int maxNumPixels);

	//! Minimum percentage of image area occupied by the blobs.
	double _minPercArea;

	//! Initial value of _minPercArea.
	static const double MIN_PERC_AREA_INITIAL_VALUE = 0.00005; //0.00005

	//! Maximum percentage of image area occupied by the blobs.
	double _maxPercArea;

	//! Initial value of _maxPercArea.
	static const double MAX_PERC_AREA_INITIAL_VALUE = 0.00400;

	//! Threshold.
	int _threshold;

	//! Initial value of threshold.
	static const int THRESHOLD_INITIAL_VALUE = 230.0;

};

#endif /* BLOBEXTRACTOR_H_ */
