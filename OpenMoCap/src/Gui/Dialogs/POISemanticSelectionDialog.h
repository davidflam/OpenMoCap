#ifndef POISEMANTICSELECTIONDIALOG_H_
#define POISEMANTICSELECTIONDIALOG_H_

#include <iostream>
#include <QApplication>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

#include "../../Entities/POI.h"
#include "../../Entities/AbstractCamera.h"
#include "../../Entities/VideoCamera.h"

class POISemanticSelectionDialog: public QDialog {

Q_OBJECT

public:

	/*!
	 * POISemanticSelectionDialog constructor.
	 *
	 * @param parent The parent dialog.
	 * @param poi The point of interest.
	 * @param mouseEventPosition The mouse event position.
	 * @param poiSemanticTypes A list with the "POI" semantic types.
	 */
	POISemanticSelectionDialog(QWidget *parent = NULL, POI *poi = NULL, QPoint mouseEventPosition = QPoint(0, 0), QStringList* poiSemanticTypes = NULL);


protected:
	/*!
	 * The widget show event handler.
	 *
	 * @param event The event to be handled.
	 */
	void showEvent(QShowEvent * event );

private slots:

	/*!
	 * Slot to refresh POI Semantic
	 */
	void refreshPOISemantic();





private:

	//! Point of interest.
	POI* _POI;

	//! OK button.
	QPushButton* _okButton;

	//! Combobox with the "POI" semantics.
	QComboBox* _POISemanticsCombo;

	//! Form layout.
	QFormLayout* _mainLayout;

	/*!
	 * Creates the main layout of the "POI" semantic selection dialog.
	 *
	 * @param poiSemanticTypes A list with the "POI" semantic types.
	 */
	void createMainLayout(QStringList* poiSemanticTypes);

	/*!
	 * The reference for the vector of the active cameras
	 */
	vector<AbstractCamera*> *_camerasRef;

};

#endif /* POISEMANTICSELECTIONDIALOG_H_ */
