/*
 * Skeleton.h
 *
 *  Created on: 05/03/2009
 *      Author: David Lunardi Flam
 */

#ifndef SKELETON_H_
#define SKELETON_H_

#include "Joint.h"

#include <iostream>
#include <map>
#include <QStringList>

#include "../Utils/Log.h"

using namespace std;

class Skeleton {

public:

	Skeleton();

	virtual ~Skeleton();

	void insertJoint(string parentJointName, Joint& joint);

	Joint& getRootJoint() {
		return _joints[_rootJointName];
	}

	Joint& getJoint(string jointName);

	QStringList* getJointsNames() {
		return &_jointsNames;
	}

	void print();

private:
	map<string, Joint> _joints;
	string _rootJointName;
	QStringList _jointsNames;
	void printJoint(Joint& joint, int depth = 0);

};

#endif /* SKELETON_H_ */
