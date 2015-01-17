#include "stdafx.h"
#include "ColladaWriter.h"

#include <fstream>
#include <sstream> 
#include "time.h"
#include "resource.h"

bool ColladaWriter::writeFile(TCHAR* path)
{
	filePath = path;
	writeHeader();
	writeMaterial();
	writeMesh();
	writeScene();
	return true;
}

bool ColladaWriter::writeHeader()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::trunc);
	if (!outFile.is_open())
	{
		throw _T("Unable to create output file.");
		return false;
	}

	HRSRC hRes = FindResource(0, MAKEINTRESOURCE(IDR_HEADER), MAKEINTRESOURCE(TEXTFILE));
	if (hRes == NULL)
	{
		throw _T("Unable to find resource.");
		return false;
	}
	
	time_t rawtime = time(NULL);
	struct tm timeInfo;
	gmtime_s(&timeInfo, &rawtime);
	int timeSize = sizeof("0000-00-00T00:00:00Z");
	char formattedTime[sizeof("0000-00-00T00:00:00Z")];
	strftime(formattedTime, timeSize, "%Y-%m-%dT%H:%M:%SZ", &timeInfo);

	HGLOBAL hData = LoadResource(0, hRes);

	if (hData == NULL)
	{
		throw _T("Unable to load resource.");
		return false;
	}

	DWORD dataSize = SizeofResource(0, hRes);
	char* data = (char*)LockResource(hData);
	char* formattedData = new char[(dataSize + (timeSize * 2))];
	sprintf_s(formattedData, dataSize + (timeSize * 2), data, formattedTime, formattedTime);
	outFile << formattedData;
	outFile.close();
	return true;
}

bool ColladaWriter::writeMaterial()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::app);
	if (!outFile.is_open())
	{
		throw _T("Unable to open output file.");
		return false;
	}

	HRSRC hRes = FindResource(0, MAKEINTRESOURCE(IDR_MATERIAL), MAKEINTRESOURCE(TEXTFILE));
	if (hRes == NULL)
	{
		throw _T("Unable to find resource.");
		return false;
	}

	HGLOBAL hData = LoadResource(0, hRes);

	if (hData == NULL)
	{
		throw _T("Unable to load resource.");
		return false;
	}

	DWORD dataSize = SizeofResource(0, hRes);
	char* data = (char*)LockResource(hData);

	outFile << "\n" << data;
	outFile.close();
	return true;
}

bool ColladaWriter::writeMesh()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::app);
	if (!outFile.is_open())
	{
		throw _T("Unable to open output file.");
		return false;
	}

	HRSRC hRes = FindResource(0, MAKEINTRESOURCE(IDR_MESH), MAKEINTRESOURCE(TEXTFILE));
	if (hRes == NULL)
	{
		throw _T("Unable to find resource.");
		return false;
	}

	HGLOBAL hData = LoadResource(0, hRes);

	if (hData == NULL)
	{
		throw _T("Unable to load resource.");
		return false;
	}

	DWORD dataSize = SizeofResource(0, hRes);
	char* data = (char*)LockResource(hData);

	ostringstream positionString, normalString, textureString, indicesString;

	for (int i = 0; i < *numVertices; i++)
	{
		positionString << vertices->at(i).positionX << " ";
		positionString << vertices->at(i).positionY << " ";
		positionString << vertices->at(i).positionZ << " ";
		normalString << vertices->at(i).normalX << " ";
		normalString << vertices->at(i).normalY << " ";
		normalString << vertices->at(i).normalZ << " ";
		textureString << vertices->at(i).textureU << " ";
		textureString << vertices->at(i).textureV << " ";
	}

	for (int i = 0; i < *numIndices; i++)
	{
		indicesString << indices->at(i) << " ";
	}

	int maxSize = dataSize + positionString.str().size() + normalString.str().size() + textureString.str().size() + indicesString.str().size() + 35;
	char* formattedData = new char[maxSize];
	sprintf_s(formattedData, maxSize, data, *numVertices * 3, positionString.str().c_str(), *numVertices, *numVertices * 3, normalString.str().c_str(), *numVertices, *numVertices * 2, textureString.str().c_str(), *numVertices, *numIndices / 3, indicesString.str().c_str());
	outFile << "\n" << formattedData;
	outFile.close();
	return true;
}

bool ColladaWriter::writeScene()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::app);
	if (!outFile.is_open())
	{
		throw _T("Unable to open output file.");
		return false;
	}

	HRSRC hRes = FindResource(0, MAKEINTRESOURCE(IDR_SCENE), MAKEINTRESOURCE(TEXTFILE));
	if (hRes == NULL)
	{
		throw _T("Unable to find resource.");
		return false;
	}

	HGLOBAL hData = LoadResource(0, hRes);

	if (hData == NULL)
	{
		throw _T("Unable to load resource.");
		return false;
	}

	DWORD dataSize = SizeofResource(0, hRes);
	char* data = (char*)LockResource(hData);

	outFile << "\n"<< data;
	outFile.close();
	return true;
}

ColladaWriter::ColladaWriter()
{
}


ColladaWriter::~ColladaWriter()
{
}
