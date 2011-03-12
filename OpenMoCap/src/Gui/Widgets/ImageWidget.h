#ifndef IMAGEWIDGET_H_
#define IMAGEWIDGET_H_

#include "../../Utils/Image.h"

#include <opencv2/opencv.hpp>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <QObject>
#include <QtOpenGL>

class ImageWidget : public QGLWidget {

Q_OBJECT

public:

	/*!
	 * ImageWidget constructor.
	 *
	 * @param parent The parent widget.
	 * @param width Image width.
	 * @param height Image height.
	 */
	ImageWidget(QWidget* parent = 0, int width = 320, int height = 240);

	/*!
	 * Refreshes the image.
	 *
	 * @param image The image to be refreshed.
	 */
	virtual void refreshImage(IplImage* image);

	/*!
	 * Returns current image
	 */
	IplImage* getImage() {
		return _imageRef;
	}

protected:

	//Initialized with NULL on constructor's initialization list.
	//! Image reference.
	IplImage *_imageRef;

	/*!
	 * Resizes the viewport.
	 *
	 * @param width Viewport width.
	 * @param height Viewport height.
	 */
	void resizeGL(int width, int height);

	/*!
	 * Image widget paint event handler.
	 *
	 * @param paintEvent The event to be handled.
	 */
	virtual void paintEvent(QPaintEvent *paintEvent);

	/*!
	 * Sets the viewport up.
	 *
	 * @param width Viewport width.
	 * @param height Viewport height.
	 */
	void setupViewPort(int width, int height);

	/*!
	 * Draws the background image.
	 */
	void drawBackgroundImage();
};

#endif /* IMAGEWIDGET_H_ */
