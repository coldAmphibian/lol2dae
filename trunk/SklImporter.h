#pragma once

class SklImporter
{
private:
	int fileVersion;
	const short& sknVersion;
public:
	int numBones;
	vector<SklBone> bones;
	int numBoneIndices;
	vector<int> boneIndices;

	void readFile(const wchar_t* path);
	SklImporter(short& sknVersion) : sknVersion(sknVersion){};
	~SklImporter();
};

