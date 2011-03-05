/*!
 * Class that implements the camera combobox.
 *
 * \name CameraComboBox
 * \author David Lunardi Flam
 * \version
 * \since 10/16/2008
 */
#include "CameraComboBox.h"

CameraComboBox::CameraComboBox(vector<AbstractCamera*> *cameras) {
	_camerasRef = cameras;

	for (int i = 0; i < (int) _camerasRef->size(); i++) {
		AbstractCamera *camera = _camerasRef->at(i);
		this->insertItem(camera->getId(), QString(camera->getName().c_str()));
	}
}

AbstractCamera* CameraComboBox::getCurrentSelectedCamera() {
	int selectedCameraIndex = this->currentIndex();
	AbstractCamera* camera = _camerasRef->at(selectedCameraIndex);

	return camera;
}

#include "CameraComboBox.moc"
