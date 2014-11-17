#include "Matrix44.h"
#include <assert.h>

CMatrix44::CMatrix44()
{
	for (int i = 0; i < MatrixSize; i++) {
		for (int j = 0; j < MatrixSize; j++) {
			elements[i][j] = i == j ? 1 : 0;
		}
	}
}

CMatrix44::CMatrix44(const double initialElements[4][4])
{
	for (int i = 0; i < MatrixSize; i++) {
		for (int j = 0; j < MatrixSize; j++) {
			elements[i][j] = initialElements[i][j];
		}
	}
}

CMatrix44::CMatrix44(const CMatrix44& otherMatrix) {
	for (int i = 0; i < MatrixSize; i++) {
		for (int j = 0; j < MatrixSize; j++) {
			elements[i][j] = otherMatrix.Get(i, j);
		}
	}
}

CMatrix44::CMatrix44(
	double a11, double a12, double a13, double a14,
	double a21, double a22, double a23, double a24,
	double a31, double a32, double a33, double a34,
	double a41, double a42, double a43, double a44
) {
	elements[0][0] = a11;
	elements[0][1] = a12;
	elements[0][2] = a13;
	elements[0][3] = a14;
	elements[1][0] = a21;
	elements[1][1] = a22;
	elements[1][2] = a23;
	elements[1][3] = a24;
	elements[2][0] = a31;
	elements[2][1] = a32;
	elements[2][2] = a33;
	elements[2][3] = a34;
	elements[3][0] = a41;
	elements[3][1] = a42;
	elements[3][2] = a43;
	elements[3][3] = a44;
}


void CMatrix44::MultiplyRight(const CMatrix44& otherMatrix) {
	double result[4][4];
	for (int i = 0; i < MatrixSize; i++) {
		for (int j = 0; j < MatrixSize; j++) {
			result[i][j] = 0;
			for (int k = 0; k < MatrixSize; k++) {
				result[i][j] += elements[i][j] * otherMatrix.Get(k, j);
			}
		}
	}
	for (int i = 0; i < MatrixSize; i++) {
		for (int j = 0; j < MatrixSize; j++) {
			elements[i][j] = result[i][j];
		}
	}
}

C3DPoint CMatrix44::ProjectPoint(const C3DPoint originPoint) const {
	C3DPoint resultPoint;
	resultPoint.X = elements[0][0] * originPoint.X + elements[1][0] * originPoint.Y + elements[2][0] * originPoint.Z + elements[3][0];
	resultPoint.Y = elements[0][1] * originPoint.X + elements[1][1] * originPoint.Y + elements[2][1] * originPoint.Z + elements[3][1];
	resultPoint.Z = elements[0][2] * originPoint.X + elements[1][2] * originPoint.Y + elements[2][2] * originPoint.Z + elements[3][2];
	return resultPoint;
}

double CMatrix44::Get(int row, int column) const {
	assert(0 <= row && row < MatrixSize && 0 <= column && column < MatrixSize);
	return elements[row][column];
}

void CMatrix44::Set(int row, int column, double value) {
	assert(0 <= row && row < MatrixSize && 0 <= column && column < MatrixSize);
	elements[row][column] = value;
}

void CMatrix44::Update(const double elements_[4][4])
{
	for (int i = 0; i < MatrixSize; i++) {
		for (int j = 0; j < MatrixSize; j++) {
			elements[i][j] = elements_[i][j];
		}
	}
}