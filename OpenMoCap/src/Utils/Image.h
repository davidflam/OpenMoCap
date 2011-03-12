/*!
 * Class that encapsulates IplImage.
 *
 * \name Image
 * \author David Lunardi Flam
 * \version
 * \since
 */
#ifndef IMAGE_H_
#define IMAGE_H_

#include <cstdio>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <opencv2/highgui/highgui_c.h>

#include "Debug.h"

using namespace std;

//! The size of the image name.
#define IMG_NAME_SIZE 60

/*!
 * Class that allows simplified access to pixels (Image(x, y, z)) and has some simplified operations. This class
 * doesn't do bounds verification, in both the construction and the pixels access. In color images, the order is:
 * B = 0, G = 1, R = 2.
 */
template<class T> class ImageWrapper {

private:

	//! The image used.
	IplImage* _image;

	//! Image name.
	char * _imageName;

	/*!
	 * Generates a name for the image.
	 */
	void generateName() {

		_imageName = new char[IMG_NAME_SIZE];
		//Retrieve time information
		time_t rawtime;
		struct tm * timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);
		sprintf(_imageName, "Image | %s", asctime(timeinfo));
	}

	/*!
	 * Releases the pointers used to reference the image.
	 */
	void releasePointers() {

		if (_image != NULL) {
			cvReleaseImage(&_image);
			_image = NULL;
		}

		if (_imageName != NULL) {
			delete[] _imageName;
			_imageName = NULL;
		}
	}

public:
	/*!
	 * Prints the image description.
	 */
	void getDescription() {

		cout << "Image Description" << endl;
		cout << "Image Name = " << _imageName;
		cout << "Dimensions = " << _image->width << "," << _image->height << " pixels" << endl;
		cout << "Depth = " << _image->depth << " bit(s)" << endl;
		cout << "Channel(s) = " << _image->nChannels << endl;
	}

	/*!
	 * ImageWrapper constructor without arguments.
	 */
	ImageWrapper() {

		_image = NULL;
		_imageName = NULL;
	}

	/*!
	 * ImageWrapper constructor with the image name as argument.
	 *
	 * @param fileName The image name as an array of char.
	 */
	ImageWrapper(const char* fileName) {

		_image = cvLoadImage(fileName, -1);
		generateName();
	}

	/*!
	 * ImageWrapper constructor with the image name as argument.
	 *
	 * @param fileName The image name as a string.
	 */
	ImageWrapper(string fileName) {

		_image = cvLoadImage(fileName.c_str(), -1);
		generateName();
	}

	/*!
	 * ImageWrapper constructor with the image as argument.
	 *
	 * @param image The image as an IplImage.
	 */
	ImageWrapper(IplImage* image) {

		_image = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
		cvCopy(image, _image, 0);
		generateName();
	}

	/*!
	 * ImageWrapper constructor with the image width, height and number of bands as arguments.
	 *
	 * @param width The image width.
	 * @param height The image height.
	 * @param bands The number of image bands.
	 */
	ImageWrapper(const int width, const int height, const int bands = 1) {

		_image = cvCreateImage(cvSize(width, height), (sizeof(T) * 8), bands);
		clear();
		generateName();
	}

	/*!
	 * ImageWrapper destructor.
	 */
	~ImageWrapper() {

		releasePointers();
	}

	/*!
	 * Gets the image.
	 *
	 * @return The image as an IplImage.
	 */
	inline IplImage* getIplImage() const {

		return _image;
	}

	/*!
	 * Gets the image width.
	 *
	 * @return The image width.
	 */
	inline const int getWidth() const {

		return _image->width;
	}

	/*!
	 * Gets the image height.
	 *
	 * @return The image height.
	 */
	inline const int getHeight() const {

		return _image->height;
	}

	/*!
	 * Gets the number of image bands.
	 *
	 * @return The number of image bands.
	 */
	inline const int getBands() const {

		return _image->nChannels;
	}

	/*!
	 * Gets the image depth.
	 *
	 * @return The image depth.
	 */
	inline const int getDepth() const {

		return _image->depth;
	}

	/*!
	 * Gets the image width step.
	 *
	 * @return The image width step.
	 */
	inline const int getWidthStep() const {

		return _image->widthStep;
	}

	/*!
	 * Gets the image data as array of unsigned char.
	 *
	 * @return The image data as an array of unsigned char.
	 */
	inline unsigned char* getImageDataAsUnsigned() {

		return reinterpret_cast<unsigned char*> (_image->imageData);
	}

	/*!
	 * Gets the image data as an array of char.
	 *
	 * @return The image data as an array of char.
	 */
	inline char* getImageData() {

		return _image->imageData;
	}

	/*!
	 * Overload of the operator "=". Copies the argument(an ImageWrapper<T> image) to the caller image.
	 *
	 * @param image The image to be copied.
	 *
	 * @return An copy of the image in the argument.
	 */
	ImageWrapper<T>& operator=(const ImageWrapper<T>& image) {

		//Check for self-assignment
		if (this == &image) {
			return *this;
		}

		//Let the OS reclaim mem
		releasePointers();

		//Initializes and copy the assigned image
		_image = cvCreateImage(cvSize(image.getWidth(), image.getHeight()), image.getDepth(), image.getBands());
		cvCopy(image.getIplImage(), _image, 0);
		generateName();

		return *this;
	}

	/*!
	 * Overload of the operator "=". Copies the argument(an IplImage image) to the caller image.
	 *
	 * @param image The image to be copied.
	 *
	 * @return An copy of the image in the argument.
	 */
	ImageWrapper<T>& operator=(IplImage *image) {

		//Check for self-assignment
		if (this->_image == image) {
			return *this;
		}

		//Let the OS reclaim mem
		releasePointers();

		//Initializes and copy the assigned image
		_image = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
		cvCopy(image, _image, 0);
		generateName();
		return *this;
	}

	/*!
	 * Overload of the operator "()". Allows a fast pixels access.
	 *
	 * @param x The pixel coordinate x.
	 * @param y The pixel coordinate y.
	 * @param z The pixel coordinate z.
	 *
	 * @return The faster pixel location.
	 */
	inline T& operator()(const int x = 0, const int y = 0, const int z = 0) {

		return reinterpret_cast<T&> (_image->imageData[y * _image->widthStep + x * _image->nChannels * sizeof(T) + (z
				* sizeof(T))]);
	}

	/*!
	 * Cleans the image.
	 */
	inline void clear() {

		cvSet(_image, cvScalarAll(0));
	}

	/*!
	 * Gets a copy of the image.
	 *
	 * @return The image copy.
	 */
	ImageWrapper<T> getCopy() {

		ImageWrapper<T> copyImage(_image->width, _image->height, _image->nChannels);
		cvCopy(_image, copyImage._image, 0);

		return copyImage;
	}

	/*!
	 * Gets the image in gray scale.
	 *
	 * @return The image in gray scale.
	 */
	ImageWrapper<T> getGrayScale() {

		ImageWrapper<T> grayImage(_image->width, _image->height, 1);

		//If it is a color image, does the conversion.
		if (_image->nChannels == 3) {
			cvCvtColor(_image, grayImage._image, CV_BGR2GRAY);
		}
		//If it is a gray scale image, returns a copy.
		else {
			cvCopy(_image, grayImage._image, 0);
		}

		return grayImage;
	}

	/*!
	 * Gets the image in color scale.
	 *
	 * @return The image in color scale.
	 */
	ImageWrapper<T> getColorScale() {

		ImageWrapper<T> colorImage(_image->width, _image->height, 3);

		//If it is a gray scale image, does the conversion.
		if (_image->nChannels == 1) {
			cvCvtColor(_image, colorImage._image, CV_GRAY2BGR);
		}
		//If it is a color image, returns a copy.
		else {
			cvCopy(_image, colorImage._image, 0);
		}

		return colorImage;
	}

	/*!
	 * Gets the image as a float one.
	 *
	 * @return The image as a float one.
	 */
	ImageWrapper<float> getFloatImage() {

		ImageWrapper<float> floatImage(_image->width, _image->height, _image->nChannels);
		floatImage.clear();
		cvConvertScale(_image, floatImage.getIplImage());

		return floatImage;
	}

	/*!
	 * Gets the image as an unsigned char one.
	 *
	 * @return The image as an unsigned char one.
	 */
	ImageWrapper<unsigned char> getUCharImage() {

		ImageWrapper<unsigned char> uCharImage(_image->width, _image->height, _image->nChannels);
		uCharImage.clear();
		cvConvertScale(_image, uCharImage.getIplImage());

		return uCharImage;
	}

	/*!
	 * Gets the number of image pixels.
	 *
	 * @return The number of image pixels.
	 */
	int getNumberOfPixels() {

		return _image->width * _image->height;
	}

	/*!
	 * Displays the image.
	 */
	void display() {

		cvNamedWindow(_imageName, 1);
		cvShowImage(_imageName, _image);
		cvWaitKey(0);
	}

	/*!
	 * Prints an image band.
	 *
	 * @param band The image band to be printed.
	 */
	void printBand(int band = 0) {

		getDescription();

		Debug::printSpace(5);
		for (int x = 0; x < (*this).getWidth(); x++) {
			cout << setw(3) << x << "|";
		}

		Debug::printNewLine();
		Debug::printSpace(4);
		for (int x = 0; x < (*this).getWidth(); x++) {
			cout << "____";
		}

		Debug::printNewLine();
		for (int y = 0; y < (*this).getHeight(); y++) {
			for (int x = 0; x < (*this).getWidth(); x++) {
				if (x == 0) {
					cout << setw(3) << y << " |";
				}
				cout << setw(3) << (double) ((*this)(x, y, band)) << " ";
			}
			Debug::printNewLine();
		}
		Debug::printNewLine();
	}
};

//! 8-bit Image
typedef ImageWrapper<unsigned char> Image;

//! 32-bit Image
typedef ImageWrapper<float> FloatImage;

#endif /*IMAGE_H_*/
