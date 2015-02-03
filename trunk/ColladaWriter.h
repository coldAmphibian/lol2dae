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
	const vector<short>& indices;
	const vector<SknVertex>& vertices;
	const vector<SklBone>& sklBones;
	const vector<int>& boneIndices;

public:
	bool includeSkl;

	bool writeFile(TCHAR* path);
	bool writeHeader();
	bool writeMaterial();
	bool writeMesh();
	bool writeSkin();
	bool writeScene();
	ColladaWriter(vector<short>& cIndices, vector<SknVertex>& cVertices, vector<SklBone>& cSklBones, vector<int>& cBoneIndices) : indices(cIndices), vertices(cVertices), sklBones(cSklBones), boneIndices(cBoneIndices){};
	~ColladaWriter();
};

