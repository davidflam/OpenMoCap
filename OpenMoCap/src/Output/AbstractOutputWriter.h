/*!
 * Abstract class for implementing mocap file writers
 *
 * \name AbstractOutputWriter
 * \author David Lunardi Flam
 * \version
 * \since 01/29/2009
 */

#ifndef ABSTRACTOUTPUTWRITER_H_
#define ABSTRACTOUTPUTWRITER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../Entities/POI.h"
#include "../Utils/Log.h"

using namespace std;

class AbstractOutputWriter {

public:

	virtual ~AbstractOutputWriter() {
		if (_fileStream.is_open()) {
			_fileStream.close();
		}
	}

	/**
	 * Add MoCap data to internal buffer
	 * @param poiData
	 */
	void addMoCapData(vector<POI> poiData) {

		if (_currentFrame == 0) {
			_fileStream.open("output.trc");
			if (_fileStream.is_open()) {
				writeMoCapFileHeader(poiData);
			} else {
				logEMERGENCY("Cannot write data to output.trc.");
			}
		} else {

			if (_fileStream.is_open()) {
				processNewMoCapData(poiData);
			} else {
				logEMERGENCY("Cannot write data to output.trc.");
			}
		}
		_currentFrame++;

	}

protected:
	virtual void processNewMoCapData(vector<POI> poiData) = 0;
	virtual void writeMoCapFileHeader(vector<POI> poiData) = 0;

	AbstractOutputWriter(int frameRate) :
		_frameRate(frameRate), _currentFrame(0) {

		_frameTime = 1000.0 / (double) frameRate;
	}

	double getCurrentCaptureTime() {
		return (_frameTime * (double) _currentFrame) / 1000.0;
	}

	int _frameRate;
	double _frameTime;
	int _currentFrame;
	string _buffer;
	ofstream _fileStream;

};

#endif /* ABSTRACTOUTPUTWRITER_H_ */
