/*
 * TrcFileWriter.cpp
 *
 *  Created on: 09/06/2009
 *      Author: David Lunardi Flam
 */

#include "TrcFileWriter.h"

void TrcFileWriter::processNewMoCapData(vector<POI> poiData) {

	_fileStream << _currentFrame << "\t" << getCurrentCaptureTime();
	for (unsigned int i = 0; i < poiData.size(); i++) {
		_fileStream << "\t" << poiData[i].getCoordinates3d().x << "\t" << poiData[i].getCoordinates3d().y << "\t"
				<< poiData[i].getCoordinates3d().z;
	}
	_fileStream << endl;
}

void TrcFileWriter::writeMoCapFileHeader(vector<POI> poiData) {

	_fileStream << "PathFileType\t4\t(X/Y/Z)\toutput.trc\n";
	_fileStream << "DataRate\tCameraRate\tNumFrames\t";
	_fileStream << "NumMarkers\tUnits\tOrigDataRate\tOrigDataStartFrame\tOrigNumFrames\n";
	_fileStream << _frameRate << "\t" << _frameRate << "\t@1\t";
	_fileStream << poiData.size() << "\tcm\t" << _frameRate << "\t0\t@1\n";

	_fileStream << "Frame#\tTime\t";

	//--- Labels
	_fileStream << poiData[0].getSemantic().toStdString();
	for (unsigned int i = 1; i < poiData.size(); i++) {
		_fileStream << "\t\t\t" << poiData[i].getSemantic().toStdString();
	}
	_fileStream << "\t\t\n\t";

	//X1, Y1, Z1, X2, ...
	for (unsigned int i = 0; i < poiData.size(); i++) {
		_fileStream << "\tX" << i << "\tY" << i << "\tZ" << i;
	}
	_fileStream << endl;

}

void TrcFileWriter::test() {

	TrcFileWriter trcWriter(50);
	vector<POI> v;
	POI p1(32.3, 33.3, 21.3);
	p1.setSemantic(QString("Quadril"));
	POI p2(11.3, 3.3, 5.3);
	p2.setSemantic(QString("Peito"));
	v.push_back(p1);
	v.push_back(p2);
	trcWriter.addMoCapData(v);
	trcWriter.addMoCapData(v);
	trcWriter.addMoCapData(v);
	trcWriter.addMoCapData(v);
	trcWriter.addMoCapData(v);
	trcWriter.addMoCapData(v);

}
