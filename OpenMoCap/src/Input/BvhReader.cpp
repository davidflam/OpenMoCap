#include "BvhReader.h"

BvhReader::BvhReader(string bvhFilePath) {

	_bvhStream.open(bvhFilePath.c_str());

	if (!_bvhStream) {
		logWARNING("Unable to open <%s>",bvhFilePath.c_str());
		//cerr << "[WARNING] Unable to open <" << bvhFilePath << ">" << endl;
	} else {
		string token = "";
		_bvhStream >> token;

		if (token != "HIERARCHY") {
			logWARNING("%s> is not a valid BVH file.", bvhFilePath.c_str());
			//cerr << "[WARNING] <" << bvhFilePath << "> is not a valid BVH file." << endl;
		}
	}
}

Skeleton BvhReader::getSkeleton() {

	parseJoint("");
	return _skeleton;

}

void BvhReader::parseJoint(string parentJointName) {

	string token = "";

	if (parentJointName == "") {
		//--- 'ROOT'
		_bvhStream >> token;
	}

	Joint joint;
	//--- Joint name
	_bvhStream >> token;
	joint.setName(token);

	//--- '{'
	_bvhStream >> token;

	CvPoint3D64f offset = parseOffset();
	joint.setOffset(offset);

	//--- 'Channels'
	_bvhStream >> token;
	//--- 'Number of Channels'
	_bvhStream >> token;
	int channels = atoi(token.c_str());
	joint.setNumberChannels(channels);

	//--- Channels' names
	for (int i = 0; i < channels; i++) {
		_bvhStream >> token;
		//		cout << token << endl;
	}

	_skeleton.insertJoint(parentJointName, joint);

	//---'JOINT' or 'END'
	_bvhStream >> token;
	if (token == "JOINT") {

		parseJoint(joint.getName());

	} else if (token == "End") {
		//--- 'Site'
		_bvhStream >> token;

		Joint endJoint;
		endJoint.setName(joint.getName().append(".end"));

		//--- '{'
		_bvhStream >> token;

		CvPoint3D64f offset = parseOffset();
		endJoint.setOffset(offset);

		_skeleton.insertJoint(joint.getName(), endJoint);

		//--- '}'
		_bvhStream >> token;
		int parentLevel = 0;
		while (token == "}" && _bvhStream >> token) {
			parentLevel++;
		}

		//---If stream ended, up one level to find no parent joint and finish parsing
		if (_bvhStream.eof()) {
			parentLevel++;
		}

		Joint parentJoint = _skeleton.getJoint(joint.getName());
		parentLevel--;
		while (parentLevel > 0) {
			parentJoint = _skeleton.getJoint(parentJoint.getParentJointName());
			parentLevel--;
		}

		if (!parentJoint.isNull()) {
			parseJoint(parentJoint.getName());
		}
	}

}

CvPoint3D64f BvhReader::parseOffset() {

	string token = "";

	//--- 'OFFSET'
	_bvhStream >> token;

	CvPoint3D64f offset;
	_bvhStream >> token;
	offset.x = atof(token.c_str());
	_bvhStream >> token;
	offset.y = atof(token.c_str());
	_bvhStream >> token;
	offset.z = atof(token.c_str());

	return offset;
}
