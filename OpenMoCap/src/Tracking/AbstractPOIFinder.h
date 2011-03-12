#ifndef ABSTRACTPOIFINDER_H_
#define ABSTRACTPOIFINDER_H_

#include <iostream>
#include <vector>
#include <QWidget>

#include "../Utils/Image.h"
#include "../Utils/Log.h"
#include "../Utils/Utils.h"
#include "../Entities/POI.h"

using namespace std;

class AbstractPOIFinder {

public:

	/*!
	 * AbstractPOIFinder constructor.
	 * When inheriting this class, one must call this constructor explicitly.
	 */
	AbstractPOIFinder();

	/*!
	 * AbstractPOIFinder destructor.
	 */
	virtual ~AbstractPOIFinder();

	/*!
	 * Detects "POIs" in an image.
	 * In order to create a new "POI" Finder, one must provide an implementation for this method.
	 *
	 * @param image The image to be used(do not alter it, since it will be used for display purposes).
	 *
	 * @return A vector with every detected "POIs" in image.
	 */
	virtual vector<POI> getPOIsInImage(IplImage *image) = 0;

protected:

};

#endif /* ABSTRACTPOIFINDER_H_ */
