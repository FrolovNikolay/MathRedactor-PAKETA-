#pragma once

// Структура, хранящая индексы точек для треугольника в трёхмерном пространстве
struct CTriangleIndex
{
	int First, Second, Third;
	CTriangleIndex(int first, int second, int third);

	// Исключение, возникающее, когда мы хотим создать треугольник с одинаковыми или отрицательными индексами
	class IncorrectIndex
	{
	};
};

