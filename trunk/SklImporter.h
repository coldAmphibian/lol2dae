#pragma once

#include "SklBone.h"
#include <vector>

using namespace std;

class SklImporter
{
private:
	int fileVersion;

public:
	const TCHAR *filePath;
	int numBones;
	vector<SklBone> bones;
	int numBoneIndices;
	const short* sknVersion;
	vector<int> boneIndices;

	SklImporter();
	~SklImporter();
	bool readFile(TCHAR* path);
};

