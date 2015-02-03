#pragma once

struct SklBone
{
	char name[32];
	int parent;
	float scale;
	float globalMatrix[16];
	float inverseMatrix[16];
	float relativeMatrix[16];
};