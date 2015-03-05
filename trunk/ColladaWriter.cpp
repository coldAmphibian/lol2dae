#include "stdafx.h"
#include "resource.h"
#include "ColladaWriter.h"

void ColladaWriter::writeFile(const wchar_t* path)
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

	return;
}

DWORD ColladaWriter::getTemplateFromResource(int resourceId, const char*& data)
{
	HRSRC hRes = FindResource(0, MAKEINTRESOURCE(resourceId), MAKEINTRESOURCE(TEXTFILE));
	if (hRes == NULL)
	{
		throw lol2daeError("Unable to find resource.");
	}

	HGLOBAL hData = LoadResource(0, hRes);

	if (hData == NULL)
	{
		throw lol2daeError("Unable to load resource.");
	}

	DWORD dataSize = SizeofResource(0, hRes);
	data = (char*)LockResource(hData);

	return dataSize;
}

bool ColladaWriter::emplaceNode(int boneId)
{
	int parent = bones.at(boneId).parent;

	if (parent == -1)
	{
		nodes.push_back(boneId);
		nodes.push_back(bones.size() + boneId);
		return true;
	}

	int parentEnd = bones.size() + parent;
	auto it = find(nodes.begin(), nodes.end(), parentEnd);

	if (it == nodes.end())
	{
		return false;
	}
	
	nodes.emplace(it, boneId);
	nodes.emplace(it, boneId + bones.size());

	return true;
}

void ColladaWriter::createNodeStringStream()
{
	int numBones = bones.size();
	string name = bones.at(nodes.front()).name;

	nodeStream << "\t\t\t<node id=\"" << name << "\" name=\"" << name << "\" sid=\"" << name << "\" type=\"JOINT\">" << endl;
	nodeStream << "\t\t\t\t<matrix sid=\"transform\">" << endl;
	nodeStream << "\t\t\t\t";

	for (int i = 0; i < 16; i++)
	{
		nodeStream << bones.at(nodes.front()).relativeMatrix[i] << " ";
	}

	nodeStream << endl << "\t\t\t\t</matrix>" << endl;

	nodes.pop_front();

	for (auto& i : nodes)
	{
		if (i >= numBones)
		{
			nodeStream << "\t\t\t</node>" << endl;
		}

		else
		{
			name = bones.at(i).name;
			nodeStream << "\t\t\t<node name=\"" << name << "\" sid=\"" << name << "\" type=\"JOINT\">" << endl;
			nodeStream << "\t\t\t\t<matrix sid=\"transform\">" << endl;
			nodeStream << "\t\t\t\t";

			for (int j = 0; j < 16; j++)
			{
				nodeStream << bones.at(i).relativeMatrix[j] << " ";
			}

			nodeStream << endl << "\t\t\t\t</matrix>" << endl;
		}
	}

	return;
}

void ColladaWriter::writeHeader()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::trunc);
	if (!outFile.is_open())
	{
		throw lol2daeError("Unable to create output file.");
	}

	DWORD dataSize = 0;
	const char* data = NULL;

	dataSize = getTemplateFromResource(IDR_HEADER, data);
	
	time_t rawtime = time(NULL);
	struct tm timeInfo;
	gmtime_s(&timeInfo, &rawtime);
	int timeSize = sizeof("0000-00-00T00:00:00Z");
	char formattedTime[sizeof("0000-00-00T00:00:00Z")];
	strftime(formattedTime, timeSize, "%Y-%m-%dT%H:%M:%SZ", &timeInfo);

	int formattedSize = dataSize + (timeSize * 2);
	char* formattedData = new char[formattedSize];

	formattedSize = sprintf_s(formattedData, formattedSize, data, formattedTime, formattedTime);
	outFile.write(formattedData, formattedSize);

	outFile.close();
	delete[] formattedData;

	return;
}

void ColladaWriter::writeMaterial()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::app);
	if (!outFile.is_open())
	{
		throw lol2daeError("Unable to open output file.");
	}

	DWORD dataSize = 0;
	const char* data = NULL;

	dataSize = getTemplateFromResource(IDR_MATERIAL, data);

	outFile << endl;
	outFile.write(data, dataSize);

	outFile.close();

	return;
}

void ColladaWriter::writeMesh()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::app);
	if (!outFile.is_open())
	{
		throw lol2daeError("Unable to open output file.");
	}
		
	DWORD dataSize = 0;
	const char* data = NULL;

	dataSize = getTemplateFromResource(IDR_MESH, data);

	ostringstream positionString, normalString, textureString, indiceString;

	for (auto& i : vertices)
	{
		positionString << i.positionX << " ";
		positionString << i.positionY << " ";
		positionString << i.positionZ << " ";
		normalString << i.normalX << " ";
		normalString << i.normalY << " ";
		normalString << i.normalZ << " ";
		textureString << i.textureU << " ";
		textureString << i.textureV << " ";
	}

	for (auto& i : indices)
	{
		indiceString << i << " ";
	}

	int maxSize = dataSize + positionString.str().size() + normalString.str().size() + textureString.str().size() + indiceString.str().size() + 70;
	char* formattedData = new char[maxSize];
	int formattedSize = sprintf_s(formattedData, maxSize, data, vertices.size() * 3, positionString.str().c_str(), vertices.size(), vertices.size() * 3, normalString.str().c_str(), vertices.size(), vertices.size() * 2, textureString.str().c_str(), vertices.size(), indices.size() / 3, indiceString.str().c_str());

	outFile << endl;
	outFile.write(formattedData, formattedSize);

	outFile.close();
	delete[] formattedData;

	return;
}

void ColladaWriter::writeSkin()
{
	ofstream outFile(filePath, ios::out | ios::binary | ios::app);
	if (!outFile.is_open())
	{
		throw lol2daeError("Unable to open output file.");
	}

	DWORD dataSize = 0;
	const char* data = NULL;

	dataSize = getTemplateFromResource(IDR_SKIN, data);

	ostringstream jointString;
	for (auto& i : boneIndices)
	{
		jointString << bones.at(i).name << " ";
	}

	ostringstream inverseMatrixString;
	for (auto& i : boneIndices)
	{
		for (int j = 0; j < 16; ++j)
		{
			inverseMatrixString << bones.at(i).inverseMatrix[j] << " ";
		}
	}

	ostringstream weightString;
	for (auto& i : vertices)
	{
		for (int j = 0; j < 4; ++j)
		{
			weightString << i.boneWeight[j] << " ";
		}
	}

	ostringstream vertexCountString;
	for (auto& i : vertices)
	{
		vertexCountString << "4 ";
	}

	ostringstream jointIndiceString;
	int k = 0;
	for (auto& i : vertices)
	{
		for (int j = 0; j < 4; ++j)
		{
			jointIndiceString << static_cast<int>(i.boneId[j]) << " " << k << " ";
			++k;
		}
	}

	int maxSize = dataSize + jointString.str().size() + inverseMatrixString.str().size() + weightString.str().size() + vertexCountString.str().size() + jointIndiceString.str().size() + 70;
	char* formattedData = new char[maxSize];
	int formattedSize = sprintf_s(formattedData, maxSize, data, boneIndices.size(), jointString.str().c_str(), boneIndices.size(), boneIndices.size() * 16, inverseMatrixString.str().c_str(), boneIndices.size(), vertices.size() * 4, weightString.str().c_str(), vertices.size() * 4, vertices.size(), vertexCountString.str().c_str(), jointIndiceString.str().c_str());

	outFile << endl;
	outFile.write(formattedData, formattedSize);

	outFile.close();
	delete[] formattedData;

	return;
}

void ColladaWriter::writeScene()
{
	if (!includeSkl)
	{
		ofstream outFile(filePath, ios::out | ios::binary | ios::app);
		if (!outFile.is_open())
		{
			throw lol2daeError("Unable to open output file.");
		}

		DWORD dataSize = 0;
		const char* data = NULL;

		dataSize = getTemplateFromResource(IDR_SCENE, data);

		outFile << endl;
		outFile.write(data, dataSize);

		outFile.close();
		return;
	}

	ofstream outFile(filePath, ios::out | ios::binary | ios::app);
	if (!outFile.is_open())
	{
		throw lol2daeError("Unable to open output file.");
	}

	DWORD dataSize = 0;
	const char* data = NULL;

	dataSize = getTemplateFromResource(IDR_SCENESKINNED, data);

	deque<int> boneIdDeque;

	for (size_t i = 0; i < bones.size(); ++i)
	{
		boneIdDeque.push_back(i);
	}

	while (!boneIdDeque.empty())
	{
		bool res = emplaceNode(boneIdDeque.front());

		if (!res)
		{
			boneIdDeque.push_back(boneIdDeque.front());
		}

		boneIdDeque.pop_front();
	}

	nodeStream.clear();
	createNodeStringStream();
	nodes.clear();

	int maxSize = dataSize + nodeStream.str().size();
	char* formattedData = new char[maxSize];
	int formattedSize = sprintf_s(formattedData, maxSize, data, nodeStream.str().c_str());
	nodeStream.clear();

	outFile << endl;
	outFile.write(formattedData, formattedSize);

	outFile.close();
	delete[] formattedData;

	return;
}

ColladaWriter::~ColladaWriter()
{
}
