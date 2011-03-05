/*!
 * Class that implements the 3D widget.
 *
 * \name ThreeDWidget
 * \author David Lunardi Flam / Carolina Andrade Silva Bigonha
 * \version
 * \since 09/08/2008
 */
#include "ThreeDWidget.h"

ThreeDWidget::ThreeDWidget(QWidget* parent, Skeleton skeleton, vector<
		AbstractCamera*>* cameras) :
	QGLWidget(parent), _floorRows(20), _floorColumns(20), _squareSize(1.0),
			_skeleton(skeleton), _rotationX(0.0), _rotationY(0.0), _camerasRef(
					cameras) {

	_eye.x = 0.0;
	_eye.y = 40.0;
	_eye.z = -40.0;

	_center.x = 0.0;
	_center.y = 0.0;
	_center.z = 0.0;

	_up.x = 0.0;
	_up.y = 1.0;
	_up.z = 0.0;

	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
	QString windowTitle("Model Visualization");
	setWindowTitle(windowTitle);

	//--- When clicked, get focus to receive keyboard events
	setFocusPolicy(Qt::ClickFocus);

	//--- On menu selection, window will show with preferred size
	_windowSelectAction = new QAction(windowTitle, this);
	connect(_windowSelectAction, SIGNAL(triggered()), this, SLOT(showNormal()));
	connect(_windowSelectAction, SIGNAL(triggered()), this, SLOT(setFocus()));

	//Initialize mouse flag
	this->_firstPosition = true;
}

QAction* ThreeDWidget::getWindowMenuAction() {
	return _windowSelectAction;
}

ThreeDWidget::~ThreeDWidget() {

}

void ThreeDWidget::initializeGL() {

	qglClearColor(Qt::black);
	glEnable(GL_DEPTH_TEST);

}

void ThreeDWidget::resizeGL(int width, int height) {

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//--- Don't allow division by zero!!!
	if (height <= 0) {
		height = 1;
	}

	GLfloat fAspect = GLfloat(width) / GLfloat(height);
	gluPerspective(60.0f, fAspect, 1.0, 400.0);

}

void ThreeDWidget::paintGL() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	drawView();
	drawFloor();
	//	drawSkeleton();
	drawPointCloud();
	drawCameras();
}

void ThreeDWidget::drawView() {

	glLoadIdentity();

	//--- Set view matrix; MODELVIEW matrix is altered but still centered at (0,0,0)
	gluLookAt(_eye.x, _eye.y, _eye.z, _center.x, _center.y, _center.z, _up.x,
			_up.y, _up.z);

	glRotatef(_rotationY, 0.0, 1.0, 0.0);
	glRotatef(_rotationX, 1.0, 0.0, 0.0);
	//--- Push matrix into the stack
	glPushMatrix();
}

void ThreeDWidget::drawFloor() {

	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, 0.0);
	glTranslatef((_floorColumns / 2) * (-_squareSize), ((_floorRows / 2)
			* _squareSize) - _squareSize, 0.0);

	for (int col = 0; col < _floorColumns; col++) {
		for (int row = 0; row < _floorRows; row++) {

			//---Light grey
			glColor3ub(133, 133, 133);

			glBegin(GL_LINE_LOOP);
			glVertex2f(0.0, 0.0);
			glVertex2f(_squareSize, 0.0);
			glVertex2f(_squareSize, _squareSize);
			glVertex2f(0.0, _squareSize);
			glEnd();

			glTranslatef(0.0, -_squareSize, 0.0);
		}
		glTranslatef(_squareSize, _floorRows * _squareSize, 0.0);
	}

}

void ThreeDWidget::drawSkeleton() {

	glPopMatrix();
	drawJoint(_skeleton.getRootJoint());
}

void ThreeDWidget::drawJoint(Joint& joint) {

	//--- Move to joint offset
	CvPoint3D64f offset = joint.getOffset();
	glTranslatef(offset.x, offset.y, offset.z);

	//--- Draw a red sphere for joint location
	GLUquadric* quadric = gluNewQuadric();
	glColor3ub(255, 0, 0);
	gluSphere(quadric, 1.0, 20, 20);

	vector<string> childrenJointsNames = joint.getChildrenJointsNames();

	for (unsigned int i = 0; i < childrenJointsNames.size(); i++) {

		Joint childJoint = _skeleton.getJoint(childrenJointsNames.at(i));
		CvPoint3D64f childJointOffset = childJoint.getOffset();

		//--- Draw a white bone between joints
		glColor3ub(255, 255, 255);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(childJointOffset.x, childJointOffset.y, childJointOffset.z);
		glEnd();

		//--- Save current position
		glPushMatrix();

		drawJoint(childJoint);

		//--- Restore current position
		glPopMatrix();

	}

	gluDeleteQuadric(quadric);

}

void ThreeDWidget::drawPointCloud() {

	glPopMatrix();

	GLUquadric* quadric = gluNewQuadric();

	for (unsigned int i = 0; i < _pointCloud.size(); i++) {

		glPushMatrix();

		CvPoint3D32f coords = _pointCloud[i].getCoordinates3d();
		glTranslatef(coords.x, coords.y, coords.z);

		glColor3ub(255, 0, 0);
		gluSphere(quadric, 0.1f, 20, 20);

		glPopMatrix();
	}

	gluDeleteQuadric(quadric);
}

void ThreeDWidget::drawCameras() {

	GLUquadric* quadric = gluNewQuadric();

	for (unsigned int i = 0; i < _camerasRef->size(); i++) {

		glPushMatrix();

		AbstractCamera* camera = _camerasRef->at(i);

		//--- Euler angles as composition of extrinsic rotations
		//--- z = g, y = b, x = a
		glRotatef((camera->getRotation().z * 180) / CV_PI, 0.0f, 0.0f, 1.0f);
		glRotatef((camera->getRotation().y * 180) / CV_PI, 1.0f, 0.0f, 0.0f);
		glRotatef((camera->getRotation().x * 180) / CV_PI, 0.0f, 0.0f, 1.0f);

		glTranslatef(camera->getTranslation().x, camera->getTranslation().y,
				camera->getTranslation().z);

		glColor3ub(0, 200, 0);
		glBegin(GL_LINE_LOOP);
		glVertex3f(-0.5f, 0.5f, 0.0f);
		glVertex3f(0.5f, 0.5f, 0.0f);
		glVertex3f(0.5f, -0.5f, 0.0f);
		glVertex3f(-0.5f, -0.5f, 0.0f);
		glEnd();

		glColor3ub(0, 200, 0);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, -0.5f);
		glVertex3f(-0.5f, 0.5f, 0.0f);
		glEnd();

		glColor3ub(0, 200, 0);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, -0.5f);
		glVertex3f(0.5f, 0.5f, 0.0f);
		glEnd();

		glColor3ub(0, 200, 0);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, -0.5f);
		glVertex3f(0.5f, -0.5f, 0.0f);
		glEnd();

		glColor3ub(0, 200, 0);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, -0.5f);
		glVertex3f(-0.5f, -0.5f, 0.0f);
		glEnd();

		glPopMatrix();

	}
	gluDeleteQuadric(quadric);

}

void ThreeDWidget::closeEvent(QCloseEvent* event) {

	//--- We will not allow user to close camera windows
	event->ignore();
	//--- We will minimize it
	showMinimized();
}

void ThreeDWidget::keyPressEvent(QKeyEvent *event) {

	switch (event->key()) {

	case Qt::Key_Left:
		_center.x-=1;
		break;
	case Qt::Key_Right:
		_center.x+=1;
		break;
	case Qt::Key_Up:
		_eye.z += 1;
		_eye.y -= 1;
		break;
	case Qt::Key_Down:
		_eye.z -= 1;
		_eye.y += 1;
		break;
	case Qt::Key_Backspace:
		//--- Reset to default view
		_rotationY = 0;
		_center.x = 0;
		_eye.z = -40.0;
		_eye.y = 40.0;
	}

	updateGL();

}

void ThreeDWidget::mouseMoveEvent(QMouseEvent *event) {
	//Have position to compare
	if (this->_firstPosition) {
		this->_firstPosition = false;
	} else {
		//Button left
		if (this->_leftPressed) {
			//Rotate in Y axis
			if (this->_oldX - event->x() < 0) {
				_rotationY += 1;
			} else if (this->_oldX - event->x() > 0) {
				_rotationY -= 1;
			}

			//Rotate in X axis
			if (this->_oldY - event->y() < 0) {
				_rotationX += 1;
			} else if (this->_oldY - event->y() > 0) {
				_rotationX -= 1;
			}
		} else {
			//Right button
			if (this->_oldY - event->y() < 0) {
				_eye.z += 1;
				_eye.y -= 1;
			} else if (this->_oldY - event->y() > 0) {
				_eye.z -= 1;
				_eye.y += 1;
			}
		}
	}

	this->_oldX = event->x();
	this->_oldY = event->y();

	updateGL();
}

void ThreeDWidget::mousePressEvent(QMouseEvent *event) {
	this->_leftPressed = event->button() == 1;
}

#include "ThreeDWidget.moc"
