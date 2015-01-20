#pragma once

#include "SklBone.h"
#include <vector>

using namespace std;

class SklImporter
{
private:
	char fileVersion[8];
	int numObjects;
	int skeletonId;

public:
	TCHAR *filePath;
	int numBones;
	vector<SklBone> bones;

	SklImporter();
	~SklImporter();
	bool readFile(TCHAR* path);
};

