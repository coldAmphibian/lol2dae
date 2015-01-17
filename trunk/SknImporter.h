#pragma once
#include <vector>
#include "SknVertex.h"

using namespace std;

class SknImporter
{
private:
	int magicNumber;
	short fileVersion;
	short matHeader;
	int numMaterials;
	int startIndex;
	int startVertex;

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

	SknImporter();
	~SknImporter();
	bool readFile(TCHAR* filePath);
};

