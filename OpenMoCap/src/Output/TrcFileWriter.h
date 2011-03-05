/*
 * TrcFileWriter.h
 *
 *  Created on: 09/06/2009
 *      Author: David Lunardi Flam
 */

#ifndef TRCFILEWRITER_H_
#define TRCFILEWRITER_H_

#include "AbstractOutputWriter.h"

class TrcFileWriter: public AbstractOutputWriter {

public:
	TrcFileWriter(int frameRate = 30): AbstractOutputWriter(frameRate) {};

	void processNewMoCapData(vector<POI> poiData);
	void writeMoCapFileHeader(vector<POI> poiData);
	static void test();


};

#endif /* TRCFILEWRITER_H_ */
