#include "stdafx.h"
#include "ColladaWriter.h"

#include <fstream>
#include <sstream> 
#include <string>
#include <deque>
#include "time.h"
#include "resource.h"
#include "matrixmaths.h"

using namespace std;

bool ColladaWriter::writeFile(TCHAR* path)
{
	filePath = path;
	writeHeader();
	writeMaterial();
	writeMesh();
	if (includeSkl)
	{
		writeSkin();
	}
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

	ostringstream positionString, normalString, textureString, indiceString;

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
		indiceString << indices->at(i) << " ";
	}

	int maxSize = dataSize + positionString.str().size() + normalString.str().size() + textureString.str().size() + indiceString.str().size() + 70;
	char* formattedData = new char[maxSize];
	sprintf_s(formattedData, maxSize, data, *numVertices * 3, positionString.str().c_str(), *numVertices, *numVertices * 3, normalString.str().c_str(), *numVertices, *numVertices * 2, textureString.str().c_str(), *numVertices, *numIndices / 3, indiceString.str().c_str());
	outFile << "\n" << formattedData;
	outFile.close();
	return true;
}

bool ColladaWriter::writeSkin()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::app);
	if (!outFile.is_open())
	{
		throw _T("Unable to open output file.");
		return false;
	}

	HRSRC hRes = FindResource(0, MAKEINTRESOURCE(IDR_SKIN), MAKEINTRESOURCE(TEXTFILE));
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

	ostringstream jointString;
	for (int i = 0; i < *numBoneIndices; i++)
	{
		jointString << sklBones->at(boneIndices->at(i)).name << " ";
	}

	ostringstream inverseMatrixString;
	for (int i = 0; i < *numBoneIndices; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			inverseMatrixString << sklBones->at(boneIndices->at(i)).inverseMatrix[j] << " ";
		}
	}

	ostringstream weightString;
	for (int i = 0; i < *numVertices; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			weightString << vertices->at(i).boneWeight[j] << " ";
		}
	}

	ostringstream vertexCountString;
	for (int i = 0; i < *numVertices; i++)
	{
		vertexCountString << "4 ";
	}

	ostringstream jointIndiceString;
	int k = 0;
	for (int i = 0; i < *numVertices; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			jointIndiceString << static_cast<int>(vertices->at(i).boneId[j]) << " " << k << " ";
			k++;
		}
	}
	int maxSize = dataSize + jointString.str().size() + inverseMatrixString.str().size() + weightString.str().size() + vertexCountString.str().size() + jointIndiceString.str().size() + 70;
	char* formattedData = new char[maxSize];
	sprintf_s(formattedData, maxSize, data, *numBoneIndices, jointString.str().c_str(), *numBoneIndices, *numBoneIndices * 16, inverseMatrixString.str().c_str(), *numBoneIndices, *numVertices * 4, weightString.str().c_str(), *numVertices * 4, *numVertices, vertexCountString.str().c_str(), jointIndiceString.str().c_str());
	outFile << "\n" << formattedData;
	return true;
}

bool ColladaWriter::writeScene()
{
	if (!includeSkl)
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

		outFile << "\n" << data;
		outFile.close();
		return true;
	}

	else
	{
		vector<string> nodeLinesString;
		vector<int> nodeLinesIndices;
		deque<int> boneIdDeque;

		for (int i = 0; i < *numBones; i++)
		{
			boneIdDeque.push_back(i);
		}

		while (!boneIdDeque.empty())
		{
			if (sklBones->at(boneIdDeque.front()).parent == -1)
			{
				nodeLinesIndices.push_back(boneIdDeque.front());
				nodeLinesIndices.push_back(boneIdDeque.front());
				ostringstream tempString;
				if (boneIdDeque.front() == 0)
				{
					tempString << "\t\t\t<node id=\"" << sklBones->at(boneIdDeque.front()).name << "\" name=\"" << sklBones->at(boneIdDeque.front()).name << "\" sid=\"" << sklBones->at(boneIdDeque.front()).name << "\" type=\"JOINT\">";
				}

				else
				{
					tempString << "\t\t\t<node name=\"" << sklBones->at(boneIdDeque.front()).name << "\" sid=\"" << sklBones->at(boneIdDeque.front()).name << "\" type=\"JOINT\">";
				}

				tempString << "<matrix sid=\"transform\">";

				for (int j = 0; j < 16; j++)
				{
					tempString << sklBones->at(boneIdDeque.front()).globalMatrix[j] << " ";
				}

				tempString << "</matrix>";
				nodeLinesString.push_back(tempString.str());
				nodeLinesString.push_back("\t\t\t</node>");
			}

			else
			{
				bool nodeStartFound = false;
				for (size_t i = 0; i < nodeLinesIndices.size(); i++)
				{
					if (nodeLinesIndices.at(i) == sklBones->at(boneIdDeque.front()).parent)
					{
						if (nodeStartFound)
						{
							auto iti = nodeLinesIndices.begin() + i;
							iti = nodeLinesIndices.emplace(iti, boneIdDeque.front());
							nodeLinesIndices.emplace(iti, boneIdDeque.front());
							auto its = nodeLinesString.begin() + i;
							ostringstream tempString;
							tempString << "\t\t\t<node name=\"" << sklBones->at(boneIdDeque.front()).name << "\" sid=\"" << sklBones->at(boneIdDeque.front()).name << "\" type=\"JOINT\">";
							tempString << "<matrix sid=\"transform\">";

							for (int j = 0; j < 16; j++)
							{
								tempString << sklBones->at(boneIdDeque.front()).relativeMatrix[j] << " ";
							}

							tempString << "</matrix>";
							its = nodeLinesString.emplace(its, tempString.str());
							nodeLinesString.emplace(its + 1, "\t\t\t</node>");

							nodeStartFound = false;
							break;
						}

						else
						{
							nodeStartFound = true;
						}
					}

					if (i >= nodeLinesIndices.size() - 1)
					{
						boneIdDeque.push_back(boneIdDeque.front());
						boneIdDeque.pop_front();
					}
				}
			}
			boneIdDeque.pop_front();
		}

		ofstream outFile(filePath, ios::out | ios::binary | ios::app);
		if (!outFile.is_open())
		{
			throw _T("Unable to open output file.");
			return false;
		}

		HRSRC hRes = FindResource(0, MAKEINTRESOURCE(IDR_SCENESKINNED), MAKEINTRESOURCE(TEXTFILE));
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

		ostringstream nodeString;

		for (size_t i = 0; i < nodeLinesString.size(); i++)
		{
			nodeString << nodeLinesString.at(i) << "\n";
		}

		int maxSize = dataSize + nodeString.str().size();
		char* formattedData = new char[maxSize];
		sprintf_s(formattedData, maxSize, data, nodeString.str().c_str());
		outFile << "\n" << formattedData;
		return true;
	}
}

ColladaWriter::ColladaWriter()
{
}


ColladaWriter::~ColladaWriter()
{
}
