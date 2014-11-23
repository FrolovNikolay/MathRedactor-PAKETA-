//�����: ����� ������
#pragma once 

#include <iostream>
#include <vector>
#include "3DPoint.h"
#include "CFormula.h"
#include "FormulaParser.h"
#include <cassert>

/*
�����: ����� ���� ��������, ��� �� ����� ���� ����� ������� ������ ��������� �� ������.
���� ����������� ������ ������������ �����, ��������, ����������� ����. �� ��� ����, �� ����.
����� ������� �������� �������� ������ � ���������.
����� ��� ���������� �������. 
� ������ ������ ��������� ������� ���������. ���� ��� ������� �� ���������� ���������, ��
����� ����������� �� ����� ','. � �������� ������� ��������� ������� ����: <id>=[<left>;<right>],
������� ��������� ������ �������� ��������, ��� ��� ��������������� ���������, ��� � ��� �������.
�������� ������ ����� ����������. ����� ��������� ����������� ������ ��� �����: eps=<value>. 
�������.
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
	//eps-�������� ���� �����
	bool buildPointGrid( const CFormula& formula, std::map< char, std::pair< double, double > > args, double eps ); //������������ �������, ������ �����, �������� ����������� �������.
	const std::vector< C3DPoint >& GetPoints() const { return points; }
	const std::vector< std::pair< int, int > >& GetSegments() const { return segments; }

private:
	std::vector< C3DPoint > points;
	std::vector< std::pair< int, int > > segments;
};