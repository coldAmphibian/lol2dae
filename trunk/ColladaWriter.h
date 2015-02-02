#pragma once
#include <vector>
#include <windows.h>
#include "FileLocation.h"
#include "SknVertex.h"
#include "SklBone.h"

using namespace std;

class ColladaWriter
{
private:
	const TCHAR* filePath;
	FileLocation outputPath;

public:
	const int* numIndices;
	const int* numVertices;
	const vector<short>* indices;
	const vector<SknVertex>* vertices;
	bool includeSkl;
	const int* numBones;
	const vector<SklBone>* sklBones;
	const int* numBoneIndices;
	const vector<int>* boneIndices;

	bool writeFile(TCHAR* filePath);
	bool writeHeader();
	bool writeMaterial();
	bool writeMesh();
	bool writeSkin();
	bool writeScene();
	ColladaWriter();
	~ColladaWriter();
};

