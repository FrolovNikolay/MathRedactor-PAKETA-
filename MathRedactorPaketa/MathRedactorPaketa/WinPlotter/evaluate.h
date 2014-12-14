//Автор: Орлов Никита
// fixed: Timur Khusaenov
#pragma once 

#include <iostream>
#include <vector>
#include "3DPoint.h"
#include "CFormula.h"
#include "FormulaParser.h"
#include "TriangleIndex.h"
#include <cassert>

/*
	класс, который по формуле и диапазону аргументов с заданным разбиением строит сетку для графика
*/
class CGraphBuilder {
public:
	//обрабатывает формулу, строит точки, проводит необходимые отрезки.
	bool buildPointGrid( const CFormula& formula, std::map< char, std::pair< double, double > > args, double eps );

	// getters
	const std::vector< C3DPoint >& GetPoints() const { return points; }
	const std::vector< std::pair< int, int > >& GetSegments() const { return segments; }
	const std::vector< CTriangleIndex >& GetTriangles() const { return triangles; }
	double GetMaxZ() const { return maxZ; }
	double GetMinZ() const { return minZ; }
private:
	std::vector< C3DPoint > points;
	std::vector< std::pair< int, int > > segments;
	std::vector< CTriangleIndex > triangles;
	double maxZ, minZ;
};