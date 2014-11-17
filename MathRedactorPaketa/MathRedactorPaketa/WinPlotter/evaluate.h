//Автор: Орлов Никита
#pragma once 

#include <iostream>
#include <vector>
#include "3DPoint.h"
#include "CFormula.h"
#include "FormulaParser.h"
#include <cassert>

/*
ВАЖНО: Автор кода осознает, что на самом деле кроме формулы ничего вводиться не должно.
Ввод параментров должен происходтить через, например, всплывающие окна. Но что есть, то есть.
Далее следует описание принцыпа работы с примерами.
Класс для построения графика. 
в первой строке ожидается формула полностью. Если она состоит из нескольких уравнений, то
нужно перечислить их через ','. В следущих строках определны отрезки вида: <id>=[<left>;<right>],
которые позволяют задать диапазон значений, как для параметрических уравнений, так и для обычных.
Ненужные данные класс игнорирует. Также добавлена возможность задать шаг через: eps=<value>. 
Примеры.
#test1
x=10*cos(l)*cos(t),y=10*cos(l)*sin(t),z=10*sin(t)
l=[-1.57;1.57]
t=[0;6.28]

#test2
x=sin(t),y=cos(t),z=l
t=[0;6.28]
eps=0.1
l=[-10;10]

*/
class CGraphBuilder {
public:
	//eps-точность шага сетки
	CGraphBuilder() : eps( 1 ),
	 	leftBound( -10. ), rightBound( 10. ) { }
	bool buildPointGrid( const std::string& data ); //обрабатывает формулу, строит точки, проводит необходимые отрезки.
	const std::vector< C3DPoint >& GetPoints() const { return points; }
	const std::vector< std::pair< int, int > >& GetSegments() const { return segments; }
private:
	double eps;
	//если выяснится, что функция параметрическая, то можно выставить диапазон параметров.
	double leftBound; 
	double rightBound; 
	std::vector< C3DPoint > points;
	std::vector< std::pair< int, int > > segments;
};