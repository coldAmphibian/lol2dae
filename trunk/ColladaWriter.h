#pragma once

class ColladaWriter
{
private:
	const wchar_t* filePath;
	FileLocation outputPath;
	const vector<short>& indices;
	const vector<SknVertex>& vertices;
	const vector<SklBone>& bones;
	const vector<int>& boneIndices;
	list<int> nodes;
	ostringstream nodeStream;

	DWORD getTemplateFromResource(int resourceId, const char*& data);
	bool emplaceNode(int boneId);
	void createNodeStringStream();
	void writeHeader();
	void writeMaterial();
	void writeMesh();
	void writeSkin();
	void writeScene();

public:
	bool includeSkl;

	void writeFile(const wchar_t* path);

	ColladaWriter(vector<short>& indices, vector<SknVertex>& vertices, vector<SklBone>& bones, vector<int>& boneIndices) 
		: indices(indices), vertices(vertices), bones(bones), boneIndices(boneIndices){};
	~ColladaWriter();
};

