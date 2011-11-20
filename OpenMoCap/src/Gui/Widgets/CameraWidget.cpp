/*!
 * Class that implements the camera widget.
 *
 * \name CameraWidget
 * \author David Lunardi Flam
 * \version
 * \since 01/29/2009
 */


#include "CameraWidget.h"


CameraWidget::CameraWidget(QWidget* parent, QStringList* POISemanticTypes, VideoController* videoController, vector<AbstractCamera*>* cameras/*, MainWindow* parent2*/) :
	QWidget(parent), _POISemanticTypesRef(POISemanticTypes), _videoController(videoController) {

	QString windowTitle = QString(_videoController->getCamera()->getName().c_str());
	setWindowTitle(windowTitle);
	createMainLayout();
	createActions();

	_videoController->setPOIsImageWidget(_POIsImageWidget);

	_camerasRef = cameras;

	//_parent = parent2;

	//parent->ggetMocapRef()->getTrackingAlgorithm();
}

CameraWidget::~CameraWidget() {

	delete _videoController;

}

void CameraWidget::createMainLayout() {

	_mainLayout = new QVBoxLayout(this);
	_mainLayout->setSizeConstraint(QLayout::SetFixedSize);

	int frameWidth = _videoController->getCamera()->getWidth();
	int frameHeight = _videoController->getCamera()->getHeight();
	_POIsImageWidget = new POIsImageWidget(this, frameWidth, frameHeight);
	_POIsImageWidget->installEventFilter(this);

	_mainLayout->addWidget(_POIsImageWidget);

	setLayout(_mainLayout);

}

void CameraWidget::contextMenuEvent(QContextMenuEvent *event) {

	QMenu contextMenu(this);

	/* TODO Settings for each camera
	QString settingsMenuLabel = QString("Settings");
    QMenu settingsMenu(settingsMenuLabel, &contextMenu);
    settingsMenu.addAction(_cameraInfoAction);
    settingsMenu.addAction(_POISettingsAction);
    settingsMenu.addAction(_trackingSettingsAction);
    contextMenu.addMenu(&settingsMenu);
    */

    if(!_videoController->getCamera()->getCameraType()) {
    	contextMenu.addAction(_loadVideoAction);
    }
	contextMenu.addAction(_saveFrameAction);
	contextMenu.addAction(_recordVideoAction);
	contextMenu.addAction(_clearPOIsAction);

    contextMenu.exec(event->globalPos());
}

void CameraWidget::createActions() {

	//--- On menu selection, window will show with preferred size
	_windowSelectAction = new QAction(windowTitle(), this);
	connect(_windowSelectAction, SIGNAL(triggered()), this, SLOT(showNormal()));
	connect(_windowSelectAction, SIGNAL(triggered()), this, SLOT(setFocus()));

	//--- Video Actions
	if(!_videoController->getCamera()->getCameraType())
	{
		_loadVideoAction = new QAction(QIcon("Resources/imageGUI/icons/configure64.png"), "Load video", this);
		connect(_loadVideoAction, SIGNAL(triggered()), this, SLOT(loadVideo()));
	}

	_saveFrameAction = new QAction(QIcon("Resources/imageGUI/icons/configure64.png"), "Save frame as...", this);
	connect(_saveFrameAction, SIGNAL(triggered()), this, SLOT(saveCurrentFrame()));

	_recordVideoAction = new QAction(QIcon("Resources/imageGUI/icons/configure64.png"), "Record video... (Click inside image to stop!)", this);
	connect(_recordVideoAction, SIGNAL(triggered()), this, SLOT(recordVideo()));

	//--- Settings
	_cameraInfoAction = new QAction(QIcon("Resources/imageGUI/icons/configure64.png"),
			"Camera...", this);
	connect(_cameraInfoAction, SIGNAL(triggered()), this, SLOT(openCameraInformationDialog()));

	_POISettingsAction = new QAction(QIcon("Resources/imageGUI/icons/configure64.png"), "POI Algorithm...",
			this);

	_trackingSettingsAction = new QAction(QIcon("Resources/imageGUI/icons/configure64.png"),
			"Tracking Algorithm...", this);

	_clearPOIsAction = new QAction(QIcon("Resources/imageGUI/icons/configure64.png"), "Clear initialized POIs", this);
	connect(_clearPOIsAction, SIGNAL(triggered()), this, SLOT(clearInitializedPOIs()));

}

void CameraWidget::clearInitializedPOIs() {

	map<string, POI> emptyPOIsMap;
	_videoController->getCamera()->setPOIs(emptyPOIsMap);
}

void CameraWidget::saveCurrentFrame() {

	//---Get current time
	SYSTEMTIME time;
	GetLocalTime(&time);

	QString fileName = QString("%1_%2%3%4_%5%6%7").arg(_videoController->getCamera()->getName().c_str()).arg(time.wDay).arg(
			time.wMonth).arg(time.wYear).arg(time.wHour).arg(time.wMinute).arg(time.wSecond);
	QString fullPath = QFileDialog::getSaveFileName(this, tr("Save Image"), fileName, tr("Image Files (*.png)"));

	//--- If user didn't cancel the dialog, an image will be save with the desired name
	if (!fullPath.isNull()) {
		cvSaveImage(fullPath.toStdString().c_str(), _POIsImageWidget->getImage());
	}
}

void CameraWidget::loadVideo(){

	QString sourceVideo = QFileDialog::getOpenFileName(this, tr("Select Video"), "C:\\Users\\Pacheco\\Documents\\VideosTestes\\Zigue-Zaque Rápidos", tr("Video Files (*.avi)"));

	((VideoCamera*)_videoController->getCamera())->setSourceVideo(sourceVideo);

	//FIXME Maybe should build a better and best located method for this
	// I think it's not very semanthic
	for(unsigned int i = 0; i < _camerasRef->size(); i++) {
		if(_camerasRef->at(i)->getCameraType() == VIDEO)
			((VideoCamera*)_camerasRef->at(i))->resetVideo();
	}

	//_parent->getMocapRef();
	_videoController->restartTracker();


}

void CameraWidget::recordVideo() {

	QString fullPath = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Video Files (*.avi)"));

	//--- If user didn't cancel the dialog, video will be saved with desired name
	if (!fullPath.isNull()) {
		_videoController->startRecordingVideo(fullPath.toStdString().c_str());
	}
}

void CameraWidget::closeEvent(QCloseEvent* event) {

	//--- We will not allow user to close camera windows
	event->ignore();
	//--- We will minimize it
	showMinimized();
}

QAction* CameraWidget::getWindowMenuAction() {
	return _windowSelectAction;
}

void CameraWidget::openCameraInformationDialog() {

	CameraInformationDialog cameraInformationDialog(this, _videoController->getCamera(),
			_videoController->getCalibrator());
	cameraInformationDialog.exec();
}

bool CameraWidget::eventFilter(QObject *target, QEvent *event) {

	if (target == _POIsImageWidget && event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *> (event);

		//--- Pauses Video Capture
		_videoController->stopVideoCapture();

		POI selectedPOI = _POIsImageWidget->getPOIAtPosition(mouseEvent->x(), mouseEvent->y());

		//--- Valid POI must have coordinates > 0
		if (selectedPOI.getCoordinates2d().x > 0) {

			//--- Removes old POI if it already has semantic
			if (selectedPOI.isInitialized()) {
				_videoController->getCamera()->removePOI(selectedPOI.getSemantic().toStdString());
			}

			QPoint dialogPosition(mouseEvent->globalX(), mouseEvent->globalY());
			POISemanticSelectionDialog POISemanticDialog(this, &selectedPOI, dialogPosition, _POISemanticTypesRef);

			//--- POI semantic selection
			POISemanticDialog.exec();

			//--- Predicted position now should be the observed position
			selectedPOI.setPredictedPosition(selectedPOI.getCoordinates2d());

			//--- Inserts POI with updated semantic
			_videoController->getCamera()->insertPOI(selectedPOI);

		}

		_videoController->startVideoCapture();
	}

	return QWidget::eventFilter(target, event);

}

#include "CameraWidget.moc"
