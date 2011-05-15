/*!
 * Class that implements the program main window.
 *
 * \name MainWindow
 * \author David Lunardi Flam
 * \author Carolina Andrade Silva Bigonha
 * \version
 * \since 09/02/2008
 */
#include "MainWindow.h"

void MainWindow::about() {
	QMessageBox::about(this, tr("About OpenMoCap"), tr("<center><b>OpenMoCap 2011</b><br/>"
		"<br/><i>Lead Developer</i>"
		"<br/>David Lunardi Flam<br/>"
		"<br/><i>Developers</i>"
		"<br/>Carolina Andrade Silva Bigonha"
		"<br/>Daniel Pacheco de Queiroz"
		"<br/>João Victor Boechat Gomide"
		"<br/>Thatyene Louise Alves de Souza Ramos"));
}

void MainWindow::createActions() {

	_aboutMocapAction = new QAction(QIcon("Resources/imageGUI/icons/about64.png"), tr("About OpenMoCap"), this);
	_aboutMocapAction->setStatusTip(tr("About OpenMoCap..."));
	connect(_aboutMocapAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenu() {

	_menuBar = new QMenuBar();

	_windowsMenu = _menuBar->addMenu("Windows");

	_aboutMenu = _menuBar->addMenu("About");
	_aboutMenu->addAction(_aboutMocapAction);

	setMenuBar(_menuBar);
}

void MainWindow::beginCapture() {

	//TODO verify if number of POIs is equal within each camera to begin calibration.

	//---We don't need to refresh time in real-time; Qt doesn't support it either.
	_timeDetail->display("00:00:00.000");
	_captureTimeRefresher->start(200);

	_stopCaptureButton->setEnabled(true);
	_beginCaptureButton->setDisabled(true);

	_captureController->setCaptureStatus(CaptureStatusEnum::RECORDING);
}

void MainWindow::stopCapture() {

	_captureTimeRefresher->stop();
	refreshCaptureTime();

	_stopCaptureButton->setDisabled(true);
	_beginCaptureButton->setEnabled(true);
	_captureController->setCaptureStatus(CaptureStatusEnum::PLAYING);
}

void MainWindow::refreshCaptureTime() {

	_timeDetail->display(_captureController->getCaptureTime().toString("hh:mm:ss:zzz"));
}

void MainWindow::createToolBars() {

	_configToolBar = addToolBar(tr("Configure"));

	//--- Record Button
	_beginCaptureButton = new QPushButton(QIcon("Resources/imageGUI/icons/start64.png"), "", this);
	_beginCaptureButton->setToolTip(tr("Begin Capture!"));
	connect(_beginCaptureButton, SIGNAL(clicked()), this, SLOT(beginCapture()));
	_configToolBar->addWidget(_beginCaptureButton);

	//--- Stop Button
	_stopCaptureButton = new QPushButton(QIcon("Resources/imageGUI/icons/stop64.png"), "", this);
	_stopCaptureButton->setToolTip(tr("Stop"));
	_stopCaptureButton->setDisabled(true);
	connect(_stopCaptureButton, SIGNAL(clicked()), this, SLOT(stopCapture()));
	_configToolBar->addWidget(_stopCaptureButton);

	//--- Stereo Calibration Button
	_stereoCalibrationButton = new QPushButton(QIcon("Resources/imageGUI/icons/cam48.png"), "", this);
	_stereoCalibrationButton->setToolTip("Start stereo calibration");
	if (_mocapRef->getCameras()->size() < 1) {
		_stereoCalibrationButton->setDisabled(true);
	}
	connect(_stereoCalibrationButton, SIGNAL(clicked()), this, SLOT(stereoCalibration()));
	_configToolBar->addWidget(_stereoCalibrationButton);

	//--- Separator
	QLabel* buttonSeparator = new QLabel(this);
	buttonSeparator->setFrameShape(QFrame::VLine);
	_configToolBar->addWidget(buttonSeparator);

	//--- Time Details
	_timeDetail = new QLCDNumber();
	_timeDetail->setNumDigits(12);
	_timeDetail->display("00:00:00.000");
	_configToolBar->addWidget(_timeDetail);

	//--- Separator
	QLabel* timeSeparator = new QLabel(this);
	timeSeparator->setFrameShape(QFrame::VLine);
	_configToolBar->addWidget(timeSeparator);

	//TODO Implement algorithms selection
	//--- Calibration Algorithms Combobox
	_calibrationLabel = new QLabel(tr("Calibration:"));
	_configToolBar->addWidget(_calibrationLabel);

	_calibrationAlgorithms = new QComboBox();
	QStringList calibList;
	calibList.push_back("Differential Evolution");
	_calibrationAlgorithms->addItems(calibList);
	_configToolBar->addWidget(_calibrationAlgorithms);

	//--- Separator
	QLabel* calibrationSeparator = new QLabel(this);
	calibrationSeparator->setFrameShape(QFrame::VLine);
	_configToolBar->addWidget(calibrationSeparator);

	//--- POI Algorithms Combobox
	_POILabel = new QLabel(tr("POI:"));
	_configToolBar->addWidget(_POILabel);

	_POIAlgorithms = new QComboBox();
	QStringList POIList;
	POIList.push_back("BlobExtractor");
	_POIAlgorithms->addItems(POIList);
	_configToolBar->addWidget(_POIAlgorithms);

	//--- Separator
	QLabel* POISeparator = new QLabel(this);
	POISeparator->setFrameShape(QFrame::VLine);
	_configToolBar->addWidget(POISeparator);

	//--- Tracking Algorithms Combobox
	_trackingLabel = new QLabel(tr("Tracking:"));
	_configToolBar->addWidget(_trackingLabel);

	_trackingAlgorithms = new QComboBox();
	QStringList trackingList;
	trackingList.push_back("Alpha-Beta Filter");
	_trackingAlgorithms->addItems(trackingList);
	_configToolBar->addWidget(_trackingAlgorithms);

	//--- Separator
	QLabel* trackingSeparator = new QLabel(this);
	trackingSeparator->setFrameShape(QFrame::VLine);
	_configToolBar->addWidget(trackingSeparator);

	//--- Reconstruction Algorithms Combobox
	_reconstructionLabel = new QLabel(tr("Reconstruction:"));
	_configToolBar->addWidget(_reconstructionLabel);

	_reconstructionAlgorithms = new QComboBox();
	QStringList reconstructionList;
	reconstructionList.push_back("Basic Reconstructor");
	_reconstructionAlgorithms->addItems(reconstructionList);
	_configToolBar->addWidget(_reconstructionAlgorithms);

	//--- Separator
	QLabel* reconstructionSeparator = new QLabel(this);
	reconstructionSeparator->setFrameShape(QFrame::VLine);
	_configToolBar->addWidget(reconstructionSeparator);

	//--- Output Algorithms Combobox
	_outputLabel = new QLabel(tr("Output:"));
	_configToolBar->addWidget(_outputLabel);

	_outputAlgorithms = new QComboBox();
	QStringList outputList;
	outputList.push_back("BVH");
	_outputAlgorithms->addItems(outputList);
	_configToolBar->addWidget(_outputAlgorithms);

	_outputConfigurationButton = new QPushButton(QIcon("Resources/imageGUI/icons/configure64.png"), "", this);
	_outputConfigurationButton->setToolTip("Configure Output Algorithm");
	_configToolBar->addWidget(_outputConfigurationButton);

	//--- Separator
	QLabel* outputSeparator = new QLabel(this);
	outputSeparator->setFrameShape(QFrame::VLine);
	_configToolBar->addWidget(outputSeparator);

	//--- Update Algorithm(s)
	_updateAlgorithmsButton = new QPushButton("Update Algorithm(s)", this);
	_configToolBar->addWidget(_updateAlgorithmsButton);

	//--- Separator
	QLabel* changeAlgorithmsSeparator = new QLabel(this);
	changeAlgorithmsSeparator->setFrameShape(QFrame::VLine);
	_configToolBar->addWidget(changeAlgorithmsSeparator);
}

void MainWindow::createStatusBar() {

	_statusBar = new QStatusBar();

	int w = _mocapRef->getCameraResolutionWidth();
	int h = _mocapRef->getCameraResolutionHeight();
	int f = _mocapRef->getCameraFrameRate();

	QString captureResolution = QString("Resolution: %1 pixels X %2 pixels").arg(w).arg(h);
	_captureDetailResolution = new QLabel(captureResolution);
	_captureDetailResolution->setFrameShape(QFrame::WinPanel);
	_captureDetailResolution->setFrameShadow(QFrame::Sunken);
	_statusBar->addPermanentWidget(_captureDetailResolution);

	QString captureFrameRate = QString("FrameRate: %1 fps").arg(f);
	_captureDetailFrameRate = new QLabel(captureFrameRate);
	_captureDetailFrameRate->setFrameShape(QFrame::WinPanel);
	_captureDetailFrameRate->setFrameShadow(QFrame::Sunken);
	_statusBar->addPermanentWidget(_captureDetailFrameRate);

	setStatusBar(_statusBar);
}

void MainWindow::createSubWindows() {

	//--- Model Visualization
	ThreeDWidget* modelVisualization = new ThreeDWidget(this, _mocapRef->getSkeleton(), _mocapRef->getCameras());
	_windowsMenu->addAction(modelVisualization->getWindowMenuAction());
	_captureController->setVisualizationRef(modelVisualization);

	QMdiSubWindow* modelVisualizationWindow = _mdiArea->addSubWindow(modelVisualization);
	modelVisualizationWindow->resize(_mocapRef->getCameraResolutionWidth(), _mocapRef->getCameraResolutionHeight());
	modelVisualizationWindow->show();
	_subWindows.push_back(modelVisualizationWindow);

	//--- Cameras' Windows
	vector<AbstractCamera*> *cameras = _mocapRef->getCameras();

	for (unsigned int i = 0; i < cameras->size(); i++) {

		VideoController* videoController = new VideoController(cameras->at(i), _mocapRef->getCalibrationAlgorithm(),
				_mocapRef->getPOIFinderAlgorithm(), _mocapRef->getTrackingAlgorithm());
		_captureController->addVideoController(videoController);

		CameraWidget* cameraWidget = new CameraWidget(this, _mocapRef->getPOISemanticTypes(), videoController, cameras);

		_windowsMenu->addAction(cameraWidget->getWindowMenuAction());

		QMdiSubWindow* cameraWindow = _mdiArea->addSubWindow(cameraWidget);
		cameraWindow->show();
		//--- User won't be able to resize camera window, since we don't want to do scale transformation on POIs coordinates
		cameraWindow->setFixedSize(cameraWindow->width(), cameraWindow->height());

		_subWindows.push_back(cameraWindow);
	}
}

MainWindow::MainWindow(Mocap *mocap) :
	_mocapRef(mocap), _stereoVisionCalibrationDialog(NULL) {

	_mdiArea = new QMdiArea();
	_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setCentralWidget(_mdiArea);

	createActions();
	createMenu();
	createStatusBar();
	createToolBars();

	_captureController = new CaptureController(_mocapRef);
	createSubWindows();
	_captureController->resume();

	_captureTimeRefresher = new QTimer();
	connect(_captureTimeRefresher, SIGNAL(timeout()), this, SLOT(refreshCaptureTime()));
	setWindowTitle(tr("OpenMoCap"));
}

void MainWindow::closeEvent(QCloseEvent *event) {

	logDEBUG("Close event received, cleaning application before closing");

	delete _captureTimeRefresher;
	delete _captureController;
	if (_stereoVisionCalibrationDialog != NULL) {
		delete _stereoVisionCalibrationDialog;
	}
	event->accept();
}

void MainWindow::stereoCalibration() {

	if (_captureController->getMocap()->getCameras()->size() == 0) {

		QMessageBox::critical(this, "Critical Error", "There are no connected cameras!");
		logERROR("Can't calibrate, there are no connected cameras!!!");

	} else if (_stereoVisionCalibrationDialog == NULL) {
		_stereoVisionCalibrationDialog = new StereoVisionCalibrationDialog(_captureController);
	}

	if (_stereoVisionCalibrationDialog != NULL) {

		_stereoVisionCalibrationDialog->show();
		_stereoVisionCalibrationDialog->raise();
		_stereoVisionCalibrationDialog->activateWindow();

	}
}

Mocap* MainWindow::getMocapRef(){
	return _mocapRef;
}


#include "MainWindow.moc"
