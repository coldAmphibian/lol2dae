#pragma once

#include "SklBone.h"
#include <vector>

using namespace std;

class SklImporter
{
private:
	const TCHAR* filePath;
	int fileVersion;

public:
	int numBones;
	vector<SklBone> bones;
	int numBoneIndices;
	const short& sknVersion;
	vector<int> boneIndices;

	SklImporter(short& cSknVersion) : sknVersion(cSknVersion){};
	~SklImporter();
	bool readFile(TCHAR* path);
};

