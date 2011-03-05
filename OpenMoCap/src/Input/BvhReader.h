/*
 * BvhReader.h
 *
 *  Created on: 04/03/2009
 *      Author: David
 */

#ifndef BVHREADER_H_
#define BVHREADER_H_

#include <fstream>
#include <iostream>
#include "../Entities/Skeleton.h"
#include "../Utils/Log.h"

using namespace std;

class BvhReader {

public:
	BvhReader() {
	}
	BvhReader(string bvhFilePath);
	Skeleton getSkeleton();

private:
	void parseJoint(string parentJointName);
	CvPoint3D64f parseOffset();
	Skeleton _skeleton;
	ifstream _bvhStream;

};

#endif /* BVHREADER_H_ */
