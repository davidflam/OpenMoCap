/*!
 * Class that implements the "POI" semantic selection dialog.
 *
 * \name POISemanticSelectionDialog
 * \author David Lunardi Flam
 * \version
 * \since 11/04/2008
 */
#include "POISemanticSelectionDialog.h"

POISemanticSelectionDialog::POISemanticSelectionDialog(QWidget *parent, POI *POI, QPoint mouseEventPosition,
		QStringList* POISemanticTypes) :
	QDialog(parent, Qt::Tool), _POI(POI) {

	createMainLayout(POISemanticTypes);

	if (!POI->isInitialized()) {
		//--- First Semantic selected
		_POISemanticsCombo->setCurrentIndex(0);
		refreshPOISemantic();
	} else {
		int index = _POISemanticsCombo->findText(POI->getSemantic());
		_POISemanticsCombo->setCurrentIndex(index);
	}

	move(mouseEventPosition);
}

void POISemanticSelectionDialog::createMainLayout(QStringList* POISemanticTypes) {

	QString POIDesc = QString("POI at (%1, %2)").arg(_POI->getCoordinates2d().x).arg(_POI->getCoordinates2d().y);
	setWindowTitle(POIDesc);

	_POISemanticsCombo = new QComboBox();
	if (POISemanticTypes != NULL) {
		_POISemanticsCombo->addItems(*POISemanticTypes);
	}
	connect(_POISemanticsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshPOISemantic()));

	_okButton = new QPushButton("Ok");
	connect(_okButton, SIGNAL(clicked()), this, SLOT(accept()));

	_mainLayout = new QFormLayout();
	_mainLayout->addRow(new QLabel("Semantic:"), _POISemanticsCombo);
	_mainLayout->addRow(_okButton);
	_mainLayout->setSizeConstraint(QLayout::SetFixedSize);

	setLayout(_mainLayout);
}

void POISemanticSelectionDialog::showEvent(QShowEvent * event) {

	//---Where should we put the dialog?
	QDesktopWidget* desktopWidget = QApplication::desktop();
	int screenNumber = desktopWidget->primaryScreen();
	int ScreenCenterX = desktopWidget->screenGeometry(screenNumber).width() / 2;
	int ScreenCenterY = desktopWidget->screenGeometry(screenNumber).height() / 2;

	if (x() > ScreenCenterX) {
		move(x() - frameGeometry().width(), y());
	}
	if (y() > ScreenCenterY) {
		move(x(), y() - frameGeometry().height());
	}
}

void POISemanticSelectionDialog::refreshPOISemantic() {

	QString semantic = _POISemanticsCombo->currentText();
	_POI->setSemantic(semantic);

}

#include "POISemanticSelectionDialog.moc"
