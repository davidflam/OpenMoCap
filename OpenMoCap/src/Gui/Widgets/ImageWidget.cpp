/*!
 * Class that implements the image widget.
 *
 * \name ImageWidget
 * \author David Lunardi Flam
 * \version
 * \since 09/15/2008
 */
#include "ImageWidget.h"

ImageWidget::ImageWidget(QWidget* parent, int width, int height) :
	QGLWidget(parent), _imageRef(NULL) {

	//--- Double buffering / Software Render
	QGLFormat fmt;
	fmt.setDoubleBuffer(true);
	fmt.setDirectRendering(false);
	setFormat(fmt);

	setAutoFillBackground(false);

	setMinimumSize(width, height);
	setMaximumSize(width, height);
}

void ImageWidget::refreshImage(IplImage* image) {

	_imageRef = image;
	update();
}

void ImageWidget::setupViewPort(int width, int height) {

	glViewport(0, 0, static_cast<GLsizei> (width), static_cast<GLsizei> (height));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0.0, static_cast<GLdouble> (width), 0.0, static_cast<GLdouble> (height));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ImageWidget::resizeGL(int width, int height) {

	setupViewPort(width, height);
}

void ImageWidget::paintEvent(QPaintEvent *paintEvent) {

	//--- Make this widget the current context for OpenGL drawing
	makeCurrent();

	//Save current state, QPainter modifies MODELVIEW matrix.
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    setupViewPort(width(), height());

	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	drawBackgroundImage();

	//Restore state after drawing pure openGL
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

	QPainter painter(this);
	painter.end();

	//--- Set OpenGL drawing context to no area
	doneCurrent();
}

void ImageWidget::drawBackgroundImage() {

	if (_imageRef != NULL) {

		//Flip image around y axis to be displayed correctly; IplImage's coordinate system is the upside-down of openGL's.
		glPixelZoom(1.0f, -1.0f);
		glRasterPos2i(0, height());
		glDrawPixels(_imageRef->width, _imageRef->height, GL_BGR_EXT, GL_UNSIGNED_BYTE, _imageRef->imageData);
	}
}

#include "ImageWidget.moc"
