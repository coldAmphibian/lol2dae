#pragma once

struct SklBone
{
	char name[32];
	int parent;
	float scale;
	float matrix[12];
};