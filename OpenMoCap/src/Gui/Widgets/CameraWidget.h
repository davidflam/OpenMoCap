#ifndef CAMERAWIDGET_H_
#define CAMERAWIDGET_H_

#include <iostream>
#include <QAction>
#include <QMouseEvent>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>

#include "POIsImageWidget.h"
#include "../../Entities/AbstractCamera.h"
#include "../../Entities/VideoCamera.h"
#include "../Dialogs/CameraInformationDialog.h"
#include "../Dialogs/POISemanticSelectionDialog.h"
#include "../../Controllers/VideoController.h"
//#include "../MainWindow.h"

using namespace std;

class CameraWidget: public QWidget {

Q_OBJECT

public:
	/*!
	 * CameraWidget Constructor
	 */
	CameraWidget(QWidget* parent = NULL, QStringList* POISemanticTypes = NULL, VideoController* videoController = NULL, vector<AbstractCamera*>* cameras = NULL/*, MainWindow* parent2 = NULL*/);
	/*!
	 * CameraWidget Destructor
	 */
	virtual ~CameraWidget();
	/*!
	 * Retrieves window menu action
	 */
	QAction* getWindowMenuAction();

protected:
	/*!
	 * On closeEvent, window should only be minimized
	 */
	void closeEvent(QCloseEvent *event);

private slots:
	/*!
	 * Opens camera information dialog with calibration options
	 */
	void openCameraInformationDialog();
	/*!
	 * Loads video on videoCamera
	 */
	void loadVideo();
	/*!
	 * Saves current frame
	 */
	void saveCurrentFrame();
	/*!
	 * Records video from camera
	 */
	void recordVideo();
	/*!
	 * Clears POIs
	 */
	void clearInitializedPOIs();

private:
	/*!
	 * Window's parent
	 */
	//MainWindow* _parent;
	/*!
	 * Point of interest semantics
	 */
	QStringList* _POISemanticTypesRef;
	/*!
	 * Controller that will update this widget with new images
	 */
	VideoController* _videoController;
	/*!
	 * Widget to display images and points of interests
	 */
	POIsImageWidget* _POIsImageWidget;
	/*!
	 * Main layout
	 */
	QVBoxLayout* _mainLayout;
	/*!
	 * The reference for the vector of the active cameras
	 */
	vector<AbstractCamera*> *_camerasRef;
	/*!
	 * Build main layout
	 */
	void createMainLayout();
	/*!
	 * When selected from menu, this widget will be focused
	 */
	QAction* _windowSelectAction;
	/*!
	 * Popup menu option to open camera information dialog
	 */
	QAction* _cameraInfoAction;
	/*!
	 * Popup menu option to configure POI algorithm settings
	 */
	QAction* _POISettingsAction;
	/*!
	 * Popup menu option to configure tracking algorithm settings
	 */
	QAction* _trackingSettingsAction;
	/*!
	 * Popup menu option to load a video into videoCamera
	 */
	QAction* _loadVideoAction;
	/*!
	 * Popup menu option to save a frame
	 */
	QAction* _saveFrameAction;
	/*!
	 * Popup menu option to record video
	 */
	QAction* _recordVideoAction;
	/*!
	 * Popup menu option to clear initialized POIs
	 */
	QAction* _clearPOIsAction;
	/*!
	 * Creates action
	 */
	void createActions();
	/*!
	 * Builds and shows context menu
	 */
	void contextMenuEvent(QContextMenuEvent *event);
	/*!
	 * Event filter to catch click events and give sematic to a point of interest
	 */
	bool eventFilter(QObject *target, QEvent *event);
};

#endif /* CAMERAWIDGET_H_ */
