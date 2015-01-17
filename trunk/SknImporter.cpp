#include "stdafx.h"
#include "SknImporter.h"
#include <fstream>
#include <vector>

using namespace std;

bool SknImporter::checkMagicNumber()
{
	if (magicNumber == 1122867)
	{
		return true;
	}

	else return false;
}

void SknImporter::offsetIndex(short& rawIndex)
{
	if (startIndex == 0 && applyOffset)
	{
		rawIndex = rawIndex++;
	}
}

bool SknImporter::readFile(TCHAR* path)
{
	filePath = path;

	ifstream inFile(filePath, ios::in | ios::binary);
	if (!inFile.is_open())
	{
		throw _T("Unable to open source file.");
		return false;
	}
	
	// Magic Number
	inFile.read(reinterpret_cast<char*>(&magicNumber), 4);

	if (!checkMagicNumber())
	{
		throw _T("Corrupted or invalid header in file.");
		return false;
	}
	
	// Header Block
	inFile.read(reinterpret_cast<char*>(&fileVersion), 2);
	inFile.read(reinterpret_cast<char*>(&matHeader), 2);

	if (fileVersion > 4)
	{
		throw _T("File version is not supported.");
		return false;
	}

	if (!matHeader == 1)
	{
		throw _T("The file could not be parsed.");
		return false;
	}

	if (fileVersion == 0)
	{
		startIndex = 0;
		startVertex = 0;
		inFile.read(reinterpret_cast<char*>(&numIndices), 4);
		inFile.read(reinterpret_cast<char*>(&numVertices), 4);
	}

	if (fileVersion > 0)
	{
		inFile.read(reinterpret_cast<char*>(&numMaterials), 4);

		// Material Block
		if (numMaterials > 0)
		{
			inFile.seekg(64, ios_base::cur);
			inFile.read(reinterpret_cast<char*>(&startVertex), 4);
			inFile.read(reinterpret_cast<char*>(&numVertices), 4);
			inFile.read(reinterpret_cast<char*>(&startIndex), 4);
			inFile.read(reinterpret_cast<char*>(&numIndices), 4);

			if (fileVersion == 4)
			{
				inFile.seekg(4, ios_base::cur);
			}
		}

		// Count Block
		int countBlockIndices, countBlockVertices;

		inFile.read(reinterpret_cast<char*>(&countBlockIndices), 4);
		inFile.read(reinterpret_cast<char*>(&countBlockVertices), 4);

		if (!(numIndices == countBlockIndices) || !(numVertices == countBlockVertices))
		{
			throw _T("Mismatch found in count block.");
			return false;
		}
	}

	if (fileVersion == 4)
	{
		inFile.seekg(48, ios_base::cur);
	}

	// Index Block
	indices.resize(numIndices);
	for (int i = 0; i < numIndices; i++)
	{
		inFile.read(reinterpret_cast<char*>(&indices[i]), 2);
		offsetIndex(indices[i]);
	}

	// Vertex Block
	vertices.resize(numVertices);
	float rawV;

	for (int i = 0; i < numVertices; i++)
	{
		// Positions
		inFile.read(reinterpret_cast<char*>(&vertices[i].positionX), 4);
		inFile.read(reinterpret_cast<char*>(&vertices[i].positionY), 4);
		inFile.read(reinterpret_cast<char*>(&vertices[i].positionZ), 4);

		// Bone Indices
		inFile.seekg(4, ios_base::cur);

		// Weights
		inFile.seekg(16, ios_base::cur);

		// Normals
		inFile.read(reinterpret_cast<char*>(&vertices[i].normalX), 4);
		inFile.read(reinterpret_cast<char*>(&vertices[i].normalY), 4);
		inFile.read(reinterpret_cast<char*>(&vertices[i].normalZ), 4);

		// UV Coordinates
		inFile.read(reinterpret_cast<char*>(&vertices[i].textureU), 4);
		inFile.read(reinterpret_cast<char*>(&rawV), 4);

		// Invert V Coordinate
		if (invertTextureV)
		{
			vertices[i].textureV = 1 - rawV;
		}
	}
inFile.close();
return true;
}

SknImporter::SknImporter()
{
}

SknImporter::~SknImporter()
{
}
