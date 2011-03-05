#ifndef POISIMAGEWIDGET_H_
#define POISIMAGEWIDGET_H_

#include "../../Tracking/AbstractPOIFinder.h"
#include "ImageWidget.h"

class POIsImageWidget: public ImageWidget {

Q_OBJECT

public:
	POIsImageWidget(QWidget* parent, int width, int height);
	~POIsImageWidget();

	void refreshImage(IplImage* image, vector<POI> POIs);
	POI getPOIAtPosition(int x, int y);

private:
	vector<POI> _POIs;

	void paintEvent(QPaintEvent *paintEvent);
	void drawPOIs(QPainter *painter);

};

#endif /* POIS	IMAGEWIDGET_H_ */
