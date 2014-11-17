#pragma once
#include "3DPoint.h"

/*
* Задаёт матрицу 4х4 для работы с преобразованиями 
*/
class CMatrix44
{
public:
	CMatrix44();
	CMatrix44(const double initialElements[4][4]);
	CMatrix44(const CMatrix44& otherMatrix);
	CMatrix44(
		double a11, double a12, double a13, double a14,
		double a21, double a22, double a23, double a24,
		double a31, double a32, double a33, double a34,
		double a41, double a42, double a43, double a44
		);

	// Домножает сущестующую матрицу на переданную справа
	void MultiplyRight(const CMatrix44& otherMatrix);

	// Умножает матрицу на вектор-строку точки (четвётрую координату дополняет единицей)
	C3DPoint ProjectPoint(const C3DPoint originPoint) const;

	// Получает элемент M[row][column]
	double Get(int row, int column) const;

	// Присваивает элемент M[row][column] = value
	void Set(int row, int column, double value);

	// Обновляет элементы
	void Update(const double elements[4][4]);

	// Размер матрицы
	static const int MatrixSize = 4;

	// Класс исключения при индексации элемента вне размеров
	class OutOfRange {

	};

private:
	// Внутренние элементы матрицы
	double elements[4][4];
};

