#ifndef CAMERACOMBOBOX_H_
#define CAMERACOMBOBOX_H_

#include <QComboBox>
#include <QObject>

#include "../../Entities/AbstractCamera.h"

class CameraComboBox: public QComboBox {

Q_OBJECT

public:

	/*!
	 * CameraComboBox constructor.
	 *
	 * @param cameras Vector containing the camera references.
	 */
	CameraComboBox(vector<AbstractCamera*>* cameras);

	/*!
	 * Gets the current selected camera.
	 *
	 * @return The reference of the selected camera.
	 */
	AbstractCamera* getCurrentSelectedCamera();

private:

	//! Vector containing the camera references.
	vector<AbstractCamera*> *_camerasRef;
};

#endif /* CAMERACOMBOBOX_H_ */
