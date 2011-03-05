/*
 * Joint.h
 *
 *  Created on: 05/03/2009
 *      Author: David Lunardi Flam
 */

#ifndef JOINT_H_
#define JOINT_H_

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace std;

class Joint {

public:

	Joint(string name = string(""));
	~Joint();
	int getNumberChannels() const {
		return numberChannels;
	}

	void setNumberChannels(int numberChannels) {
		this->numberChannels = numberChannels;
	}

	vector<string> getChildrenJointsNames() const {
		return _childrenJointsNames;
	}

	string getName() const {
		return _name;
	}

	void setName(string _name) {
		this->_name = _name;
	}

	CvPoint3D64f getRotation() const {
		return _rotation;
	}

	void setRotation(CvPoint3D64f _rotation) {
		this->_rotation = _rotation;
	}

	CvPoint3D64f getOffset() const {
		return _offset;
	}

	void setOffset(CvPoint3D64f offset) {
		this->_offset = offset;
	}

	void insertChildJointName(string childJointName) {
		_childrenJointsNames.push_back(childJointName);
	}

	string getParentJointName() const {
		return _parentJointName;
	}

	void setParentJointName(string parentJointName) {
		this->_parentJointName = parentJointName;
	}

	bool isNull() {
		return (_name == "");
	}

private:
	string _name;
	CvPoint3D64f _rotation;
	CvPoint3D64f _offset;

	vector<string> _childrenJointsNames;
	string _parentJointName;
	int numberChannels;

};

#endif /* JOINT_H_ */
