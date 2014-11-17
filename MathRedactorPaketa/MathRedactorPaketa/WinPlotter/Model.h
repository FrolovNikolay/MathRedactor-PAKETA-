#pragma once

#include <vector>

#include "2DPoint.h"
#include "3DPoint.h"
#include "SegmentIndex.h"
#include "TriangleIndex.h"

/*
* Класс-контейнер, который хранит информацию о трЄхмерном объекте, который будет отображатьс¤ в окне просмотра,
* или его двумерной проекции
*/
template < typename T >
class CModel
{
public:
	// note: Я намеренно вынес эти структуры как public, чтобы к ним можно было обратитьс¤
	// по итераторам дл¤ последующей модификации

	// Узлы трехмерной модели
	std::vector<T> Points;

	// Индексы, определяющие отрезки
	std::vector<CSegmentIndex> Segments;

	// Индексы, определяющие треугольники
	std::vector<CTriangleIndex> Triangles;

	CModel<T>() {};

	// Добавляет узел модели в конец списка (чтобы не нарушить нумерацию)
	void AddPoint(T point)
	{
		Points.push_back(point);
	};

	// Добавляет отрезок, соединяющий две вершины
	void AddSegment(int firstIndex, int secondIndex)
	{
		if( firstIndex >= Points.size() || secondIndex >= Points.size() ) {
			throw CModel<T>::OutOfRange();
		} else {
			Segments.push_back(CSegmentIndex(firstIndex, secondIndex));
		}
	}

	// Добавляет треугольник, соединяющий три вершины
	void AddTriangle(int firstIndex, int secondIndex, int thirdIndex)
	{
		if (firstIndex >= Points.size() || secondIndex >= Points.size() || thirdIndex >= Points.size()) {
			throw CModel<T>::OutOfRange();
		}
		else {
			Triangles.push_back(CTriangleIndex(firstIndex, secondIndex, thirdIndex));
		}
	}

	// Очищает структуру
	void Clear()
	{
		Points.clear();
		Segments.clear();
		Triangles.clear();
	};

	// Исключение, которое происходит при попытке добавления отрезка или треугольника с несуществующими индексами
	class OutOfRange {
	};
};

// Конкретные классы для двухмерной и трехмерной модели соответственно
typedef CModel<C2DPoint> C2DModel;
typedef CModel<C3DPoint> C3DModel;