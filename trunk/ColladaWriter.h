#pragma once
#include <vector>
#include <windows.h>
#include "FileLocation.h"
#include "SknVertex.h"

using namespace std;

class ColladaWriter
{
private:
	TCHAR* filePath;
	FileLocation outputPath;

public:
	int* numIndices;
	int* numVertices;
	vector<short>* indices;
	vector<SknVertex>* vertices;

	bool writeFile(TCHAR* filePath);
	bool writeHeader();
	bool writeMaterial();
	bool writeMesh();
	bool writeScene();
	ColladaWriter();
	~ColladaWriter();
};

