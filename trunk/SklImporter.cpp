#include "stdafx.h"
#include "SklImporter.h"
#include "MatrixMath.h"

void SklImporter::readFile(const wchar_t* path)
{
	ifstream inFile(path, ios::in | ios::binary);
	if (!inFile.is_open())
	{
		throw lol2daeError("Unable to open skeleton file, please place it in the same directory as your input file.");
	}

	inFile.seekg(4, ios_base::cur);
	inFile.read(reinterpret_cast<char*>(&fileVersion), 4);

	if (fileVersion != 1953262451 && fileVersion != 587026371)
	{
		throw  lol2daeError("Skeleton file is invalid or unsupported.");
	}

	if (fileVersion == 1953262451)
	{
		inFile.seekg(8, ios_base::cur);
		inFile.read(reinterpret_cast<char*>(&numBones), 4);

		bones.resize(numBones);

		for (int i = 0; i < numBones; ++i)
		{
			inFile.read(reinterpret_cast<char*>(&bones.at(i).name), 32);
			inFile.read(reinterpret_cast<char*>(&bones.at(i).parent), 4);
			inFile.read(reinterpret_cast<char*>(&bones.at(i).scale), 4);
			inFile.read(reinterpret_cast<char*>(&bones.at(i).globalMatrix), 48);
			bones.at(i).globalMatrix[12] = 0;
			bones.at(i).globalMatrix[13] = 0;
			bones.at(i).globalMatrix[14] = 0;
			bones.at(i).globalMatrix[15] = 1;
		}

		if (sknVersion == 0 || sknVersion == 1)
		{
			numBoneIndices = numBones;
			boneIndices.resize(numBoneIndices);
			for (int i = 0; i < numBoneIndices; ++i)
			{
				boneIndices.at(i) = i;
			}
		}

		else if (sknVersion == 2)
		{
			inFile.read(reinterpret_cast<char*>(&numBoneIndices), 4);

			boneIndices.resize(numBoneIndices);

			for (int i = 0; i < numBoneIndices; ++i)
			{
				inFile.read(reinterpret_cast<char*>(&boneIndices.at(i)), 4);
			}
		}

		for (auto& i : bones)
		{
			MatrixMath::invertMatrix(i.globalMatrix, i.inverseMatrix);
		}

		for (auto& i : bones)
		{
			if (i.parent != -1)
			{
				MatrixMath::multiplyMatrix(bones.at(i.parent).inverseMatrix, i.globalMatrix, i.relativeMatrix);
			}

			else
			{
				for (int j = 0; j < 16; ++j)
				{
					i.relativeMatrix[j] = i.globalMatrix[j];
				}
			}
		}
	}

	else if (fileVersion == 587026371)
	{
		inFile.seekg(6, ios_base::cur);
		inFile.read(reinterpret_cast<char*>(&numBones), 2);
		numBones = short(numBones);
		bones.resize(numBones);

		inFile.read(reinterpret_cast<char*>(&numBoneIndices), 4);
		boneIndices.resize(numBoneIndices);

		short dataOffset;
		inFile.read(reinterpret_cast<char*>(&dataOffset), 2);
		inFile.seekg(2, ios_base::cur);

		int boneIndicesOffset, boneNamesOffset;
		inFile.seekg(4, ios_base::cur);
		inFile.read(reinterpret_cast<char*>(&boneIndicesOffset), 4);
		inFile.seekg(8, ios_base::cur);
		inFile.read(reinterpret_cast<char*>(&boneNamesOffset), 4);

		inFile.seekg(dataOffset, ios_base::beg);

		for (int i = 0; i < numBones; ++i)
		{
			inFile.seekg(2, ios_base::cur);
			short boneId;
			inFile.read(reinterpret_cast<char*>(&boneId), 2);

			if (boneId != i)
			{
				throw  lol2daeError("Read error, unexpected identification value.");
			}

			inFile.read(reinterpret_cast<char*>(&bones[i].parent), 2);
			bones[i].parent = (short)bones[i].parent;

			inFile.seekg(10, ios_base::cur);

			float tx, ty, tz;
			inFile.read(reinterpret_cast<char*>(&tx), 4);
			inFile.read(reinterpret_cast<char*>(&ty), 4);
			inFile.read(reinterpret_cast<char*>(&tz), 4);
			inFile.seekg(12, ios_base::cur);

			float x, y, z, w;
			inFile.read(reinterpret_cast<char*>(&x), 4);
			inFile.read(reinterpret_cast<char*>(&y), 4);
			inFile.read(reinterpret_cast<char*>(&z), 4);
			inFile.read(reinterpret_cast<char*>(&w), 4);

			MatrixMath::quaternionToMatrix(x, y, z, w, bones.at(i).relativeMatrix);
			bones.at(i).relativeMatrix[3] = tx;
			bones.at(i).relativeMatrix[7] = ty;
			bones.at(i).relativeMatrix[11] = tz;

			inFile.seekg(44, ios_base::cur);
		}

		inFile.seekg(boneIndicesOffset, ios_base::beg);

		for (int i = 0; i < numBoneIndices; ++i)
		{
			inFile.read(reinterpret_cast<char*>(&boneIndices.at(i)), 2);
			boneIndices.at(i) = short(boneIndices.at(i));
		}

		inFile.seekg(boneNamesOffset, ios_base::beg);

		for (int i = 0; i < numBones; ++i)
		{
			unsigned char curChar;
			streamoff filePos;

			do
			{
				filePos = inFile.tellg();
				inFile.read(reinterpret_cast<char*>(&curChar), 1);
			} while (filePos % 4 != 0);

			int curPos = 0;

			while (curChar)
			{
				bones.at(i).name[curPos] = curChar;
				++curPos;

				if (curPos >= 31)
				{
					break;
				}

				inFile.read(reinterpret_cast<char*>(&curChar), 1);
			}
		}

		for (auto& i : bones)
		{
			if (i.parent != -1)
			{
				MatrixMath::multiplyMatrix(bones.at(i.parent).globalMatrix, i.relativeMatrix, i.globalMatrix);
			}

			else
			{
				for (int j = 0; j < 16; ++j)
				{
					i.globalMatrix[j] = i.relativeMatrix[j];
				}
			}
		}

		for (auto& i : bones)
		{
			MatrixMath::invertMatrix(i.globalMatrix, i.inverseMatrix);
		}
	}
	inFile.close();
	return;
}

SklImporter::~SklImporter()
{
}
