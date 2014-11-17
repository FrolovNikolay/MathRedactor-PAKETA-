// Copyright (С) ABBYY (BIT Software), 1993 - 2014. All rights reserved. 
// Автор: Кирилл Яныкин, МФТИ, группа 196
// Описание: структура, задающая точку в трёхмерном пространстве (вектор-строка)
#pragma once

struct C3DPoint {
	// Координаты точки
	double X, Y, Z;

	// Конструктор по умолчанию
	C3DPoint( double _x = 0, double _y = 0, double _z = 0 );
	// Сумма векторов
	C3DPoint operator+( const C3DPoint& other ) const;
	void operator+=( const C3DPoint& other );
	// Разность векторов
	C3DPoint operator-( const C3DPoint& other ) const;
	void operator-=( const C3DPoint& other );
	// Умножение на число справа
	C3DPoint operator*( double value ) const;
	void operator*=( double value );
	// Деление на число справа
	C3DPoint operator/( double value ) const;
	void operator/=( double value );

	// Умножение на число слева
	friend C3DPoint operator*( double value, const C3DPoint& other );

	// Длина вектора
	double length() const;
	// Возвращает нормализованный вектор
	C3DPoint normalize() const;
	// Скалярное произведение
	double dot( const C3DPoint& other ) const;
	// Векторное произведение
	C3DPoint cross( const C3DPoint& other ) const;

	// Класс исключения, когда вектор внезапно имеет нулевую длину
	class CNullLengthException {
	};
};

