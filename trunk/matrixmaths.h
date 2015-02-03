#pragma once

bool invertMatrix(const float m[16], float invOut[16]);
bool multiplyMatrix(const float a[16], const float b[16], float r[16]);
bool quaternionToMatrix(const float x, const float y, const float z, const float w, float matrix[16]);
