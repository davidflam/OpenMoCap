/*
 * Skeleton.cpp
 *
 *  Created on: 05/03/2009
 *      Author: David Lunardi Flam
 */

#include "Skeleton.h"

Skeleton::Skeleton() {

}

Skeleton::~Skeleton() {

}

void Skeleton::insertJoint(string parentJointName, Joint& newJoint) {

	_jointsNames.push_back(QString(newJoint.getName().c_str()));
	_joints[newJoint.getName()] = newJoint;

	//--- Inserting non-root, update children list from parent joint
	if (!parentJointName.empty()) {

		_joints[newJoint.getName()].setParentJointName(parentJointName);
		_joints[parentJointName].insertChildJointName(newJoint.getName());

	} else {
		//--- Just one root Joint
		_rootJointName = newJoint.getName();

	}

}

Joint& Skeleton::getJoint(string jointName) {

	return _joints[jointName];
}

void Skeleton::print() {

	printJoint(getRootJoint());

}

//---Preorder
void Skeleton::printJoint(Joint& joint, int depth) {
	string logOutput = "";

	for (int i = 0; i < depth; i++) {
		logOutput += "\t";
	}
	logOutput += joint.getName();
	logOutput += "\n";
	for (int i = 0; i < depth; i++) {
		logOutput += "\t";
	}
	logDEBUG("%s(%d,%d,%d)",logOutput.c_str());

	vector<string> childrenJointsNames = joint.getChildrenJointsNames();

	for (unsigned int i = 0; i < childrenJointsNames.size(); i++) {
		printJoint(getJoint(childrenJointsNames.at(i)), depth + 1);
	}

}
