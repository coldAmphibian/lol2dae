#pragma once
#include <vector>
#include "SknVertex.h"
#include "SknBone.h"

using namespace std;

class SknImporter
{
private:
	int magicNumber;
	short fileVersion;
	short matHeader;
	int numMaterials;

	bool checkMagicNumber();
	void offsetIndex(short& rawIndex);

public:
	TCHAR *filePath;
	bool applyOffset = true;
	bool invertTextureV = true;
	int numIndices;
	int numVertices;
	vector<short> indices;
	vector<SknVertex> vertices;
	vector<SknBone> bones;

	SknImporter();
	~SknImporter();
	bool readFile(TCHAR* path);
};

