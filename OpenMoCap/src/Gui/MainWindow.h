#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QObject>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <sstream>

#include "Dialogs/CameraInformationDialog.h"
#include "Dialogs/StereoVisionCalibrationDialog.h"
#include "Widgets/CameraWidget.h"
#include "Widgets/ThreeDWidget.h"
#include "../Controllers/CaptureController.h"
#include "../Utils/Log.h"

//--- Forward Declarations
class Mocap;
class CaptureController;

using namespace std;

class MainWindow: public QMainWindow {

Q_OBJECT

public:

	/*!
	 * MainWindow constructor
	 *
	 * @param mocap Mocap reference.
	 */
	MainWindow(Mocap* mocap);


	//! Return the Mocap reference.
	Mocap* getMocapRef();

private slots:

	/*!
	 * About message box.
	 */
	void about();

	/*!
	 * Begins a new capture, refreshing the components related to the capture process.
	 */
	void beginCapture();

	/*!
	 * Stops the current capture, refreshing the components related to the capture process.
	 */
	void stopCapture();

	/*!
	 * Refreshes the capture time.
	 */
	void refreshCaptureTime();

	/*!
	 * Starts stereo calibration
	 */
	void stereoCalibration();

private:

	//Must not be freed on destructor method
	//! Mocap reference.
	Mocap* _mocapRef;

	//! Status bar.
	QStatusBar* _statusBar;

	//! Label displaying the capture resolution.
	QLabel* _captureDetailResolution;

	//! Label displaying the capture frame rate.
	QLabel* _captureDetailFrameRate;

	/*!
	 * Creates the status bar.
	 */
	void createStatusBar();

	/*!
	 * Releases the status bar.
	 */
	void releaseStatusBar();

	//! Menu bar.
	QMenuBar* _menuBar;

	//! Windows menu.
	QMenu* _windowsMenu;

	//! About menu.
	QMenu* _aboutMenu;

	/*!
	 * Creates the menu.
	 */
	void createMenu();

	/*!
	 * Releases the menu.
	 */
	void releaseMenu();

	/* Actions */
	//! Action that opens About message box.
	QAction* _aboutMocapAction;

	/*!
	 * Creates the actions.
	 */
	void createActions();

	/*!
	 * Releases the actions.
	 */
	void releaseActions();

	/* ToolBar */

	//! Configurations tool bar.
	QToolBar* _configToolBar;

	//! Begin capture button.
	QPushButton* _beginCaptureButton;

	//! Stop capture button.
	QPushButton* _stopCaptureButton;

	//! Begin stereo calibration.
	QPushButton* _stereoCalibrationButton;

	//! LCD display with the capture time.
	QLCDNumber* _timeDetail;

	//! Timer that refreshes the capture time.
	QTimer* _captureTimeRefresher;

	//! Calibration label.
	QLabel* _calibrationLabel;

	//! Combo box containing the calibration algorithms.
	QComboBox* _calibrationAlgorithms;

	//! "POI" label.
	QLabel* _POILabel;

	//! Combo box containing the "POI" finder algorithms.
	QComboBox* _POIAlgorithms;

	//! Tracking label.
	QLabel* _trackingLabel;

	//! Combo box containing the tracking algorithms.
	QComboBox* _trackingAlgorithms;

	//! Reconstruction label.
	QLabel* _reconstructionLabel;

	//! Combo box containing the reconstructions algorithms.
	QComboBox* _reconstructionAlgorithms;

	//! Output label.
	QLabel* _outputLabel;

	//! Combo box containing the output algorithms.
	QComboBox* _outputAlgorithms;

	//! Output configuration button.
	QPushButton* _outputConfigurationButton;

	//! Update algorithms button.
	QPushButton* _updateAlgorithmsButton;

	/*!
	 * Creates the tool bars.
	 */
	void createToolBars();

	/*!
	 * Releases the tool bars.
	 */
	void releaseToolBars();

	/* Central Widget */

	//! MDI area where the sub windows are displayed.
	QMdiArea* _mdiArea;

	//--- Sub windows

	/*!
	 * Creates the sub windows.
	 */
	void createSubWindows();

	//! Vector containing the sub windows instances.
	vector<QMdiSubWindow*> _subWindows;

	//! Thread for controlling capture.
	CaptureController* _captureController;

	//! Stereo Calibration
	StereoVisionCalibrationDialog* _stereoVisionCalibrationDialog;

	//! Clean application before closing
    void closeEvent(QCloseEvent *event);

};

#endif /* MAINWINDOW_H_ */
