#pragma once
class MatrixMath
{
public:
	static void invertMatrix(const float m[16], float im[16]);
	static void multiplyMatrix(const float a[16], const float b[16], float r[16]);
	static void quaternionToMatrix(const float x, const float y, const float z, const float w, float matrix[16]);
	MatrixMath();
	~MatrixMath();
};

