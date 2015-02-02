#pragma once
#include <vector>
#include "SknVertex.h"

using namespace std;

class SknImporter
{
private:
	int magicNumber;
	short matHeader;
	int numMaterials;

	bool checkMagicNumber();
	void offsetIndex(short& rawIndex);

public:
	const TCHAR *filePath;
	bool applyOffset;
	bool invertTextureV;
	int numIndices;
	int numVertices;
	vector<short> indices;
	vector<SknVertex> vertices;
	short fileVersion;

	SknImporter();
	~SknImporter();
	bool readFile(TCHAR* path);
};

