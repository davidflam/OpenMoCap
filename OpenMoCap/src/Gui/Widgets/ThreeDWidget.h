#ifndef THREEDWIDGET_H_
#define THREEDWIDGET_H_

#include <QAction>
#include <QObject>
#include <QGLWidget>
#include <QCloseEvent>
#include <qnamespace.h>
#include <iostream>

#include "../../Entities/AbstractCamera.h"
#include "../../Entities/Skeleton.h"
#include "../../Entities/POI.h"

using namespace std;

class ThreeDWidget: public QGLWidget {

Q_OBJECT

public:

	/*!
	 * ThreeDWidget constructor.
	 */
	ThreeDWidget(QWidget* parent = NULL, Skeleton skeleton = Skeleton(), vector<AbstractCamera*>* cameras = NULL);

	/*!
	 * ThreeDWidget destructor.
	 */
	~ThreeDWidget();
	/*!
	 * Return action to select window
	 */
	QAction* getWindowMenuAction();

	/*!
	 * Sets point cloud
	 */
	void setPointCloud(vector<POI> pointCloud) {
		_pointCloud = pointCloud;
	}

	/*!
	 * Returns point cloud
	 */
	vector<POI> getPointCloud() {
		return _pointCloud;
	}

protected:
	/*!
	 * Close event reimplementation, window will be minimized
	 */
	void closeEvent(QCloseEvent* event);

	/*!
	 * Handle zoom, rotation and translation
	 */
	void keyPressEvent(QKeyEvent *event);

	/*!
	 * Handle zoom, rotation and translation
	 */
	void mouseMoveEvent(QMouseEvent *event);

	/*!
	 * Handle the button clicked
	 */
	void mousePressEvent(QMouseEvent *event);
private:

	//! Number of rows present in the floor
	int _floorRows;
	//! Number of columns present in the floor
	int _floorColumns;
	//! Size of squares present in floor
	double _squareSize;
	//! Skeleton model to be used
	Skeleton _skeleton;
	//! Viewer parameter - eye
	CvPoint3D64f _eye;
	//! Viewer parameter - center
	CvPoint3D64f _center;
	//! Viewer parameter - up
	CvPoint3D64f _up;
	//! Rotation around 'X' axis
	double _rotationX;
	//! Rotation around 'Y' axis
	double _rotationY;
	//!Action to select window
	QAction* _windowSelectAction;
	//! Point cloud
	vector<POI> _pointCloud;
	//! Cameras
	vector<AbstractCamera*>* _camerasRef;
	//!Old value of x  of themouse position
	int _oldX;
	//!Old value of y of the mouse position
	int _oldY;
	//!Flag if the
	bool _firstPosition;
	//!Flag to left mouse click pressed
	bool _leftPressed;
	/*!
	 * Initializes OpenGL, just called once, before paintGL
	 */
	void initializeGL();

	/*!
	 * Draws everything, called every refresh
	 */
	void paintGL();

	/*!
	 * Resizes viewport if needed, called every refresh
	 */
	void resizeGL(int width, int height);

	/*!
	 * Draws main view
	 */
	void drawView();

	/*!
	 * Draws ambient floor, chess pattern
	 */
	void drawFloor();

	/*!
	 * Draws skeleton; will recursively call drawJoint to complete skeleton
	 */
	void drawSkeleton();

	/*!
	 * Draws joints recursively.
	 */
	void drawJoint(Joint& joint);

	/*!
	 * Draws point cloud
	 */
	void drawPointCloud();

	/*!
	 * Draws cameras
	 */
	void drawCameras();

};

#endif /* THREEDWIDGET_H_ */
