#include "stdafx.h"
#include "SklImporter.h"
#include <fstream>
#include <vector>

bool SklImporter::readFile(TCHAR* path)
{
	filePath = path;

	ifstream inFile(filePath, ios::in | ios::binary);
	if (!inFile.is_open())
	{
		throw _T("Unable to open skeleton file.");
		return false;
	}

	inFile.read(reinterpret_cast<char*>(&fileVersion), 8);
	inFile.read(reinterpret_cast<char*>(&numObjects), 4);
	inFile.read(reinterpret_cast<char*>(&skeletonId), 4);
	inFile.read(reinterpret_cast<char*>(&numBones), 4);
	inFile.close();
	return true;
}

SklImporter::SklImporter()
{
}


SklImporter::~SklImporter()
{
}
