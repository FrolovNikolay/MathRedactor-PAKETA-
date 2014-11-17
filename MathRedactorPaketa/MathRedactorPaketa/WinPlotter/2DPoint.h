#pragma once

// Структура, задающая точку в двухмерном пространстве (вектор-строка)
struct C2DPoint
{
public:
	double X, Y;

	C2DPoint(double _x = 0, double _y = 0);
};