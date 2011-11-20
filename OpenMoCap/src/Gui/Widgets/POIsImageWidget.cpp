/*!
 * Class that implements the "POIs" image widget.
 *
 * \name POIsImageWidget
 * \author David Lunardi Flam
 * \version
 * \since 10/27/2008
 */
#include "POIsImageWidget.h"

POIsImageWidget::POIsImageWidget(QWidget* parent, int width, int height) :
	ImageWidget(parent, width, height) {
}

POIsImageWidget::~POIsImageWidget() {

}

void POIsImageWidget::drawPOIs(QPainter *painter) {

	for (unsigned int i = 0; i < _POIs.size(); i++) {
		_POIs.at(i).draw2d(painter);
	}

}

void POIsImageWidget::paintEvent(QPaintEvent *paintEvent) {

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

	QPainter painter;
	painter.begin(this);
	drawPOIs(&painter);
	painter.end();

	//--- Set OpenGL drawing context to no area
	doneCurrent();

}

void POIsImageWidget::refreshImage(IplImage* image, vector<POI> POIs) {

	_imageRef = image;
	_POIs = POIs;
	update();

}

POI POIsImageWidget::getPOIAtPosition(int x, int y) {

	for (unsigned int i = 0; i < _POIs.size(); i++) {
		if (_POIs.at(i).isPointInsideSelectionArea2d(x, y)) {
			return _POIs.at(i);
		}
	}
	return POI(-1.0f, -1.0f);
}

#include "POIsImageWidget.moc"
