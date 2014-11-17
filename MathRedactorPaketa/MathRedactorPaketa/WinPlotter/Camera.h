// Copyright (С) ABBYY (BIT Software), 1993 - 2014. All rights reserved. 
// Автор: Кирилл Яныкин, МФТИ, группа 196
// Описание: Класс, реализующий модель UVN-камеры для представления в трёхмерном пространстве
#pragma once
#include "3DPoint.h"
#include "Matrix44.h"

class CCamera {

public:
	// Конструктор по умолчанию
	CCamera();
	// Проецирует координаты исходной точки originPoint в координаты системы камеры и записывает их в modifiedPoint
	void ProjectPoint(const C3DPoint& originPoint, C3DPoint& modifiedPoint) const;
	// Устанавливает камеру в заданную точку трёхмерного пространства
	void SetPosition(const C3DPoint& positionPoint);
	// Перемещает камеру вперёд (при положительном значении speed) и назад (при отрицательном)
	void MoveForward(double speed = 1.0f);
	// Перемещает камеру вверх (при положительном значении speed) и вниз (при отрицательном)
	void MoveUp(double speed = 1.0f);
	// Перемещает камеру вправо (при положительном значении speed) и влево (при отрицательном)
	void MoveSide(double speed = 1.0f);

private:
	C3DPoint position; // Положение камеры в трёхмерном пространстве	
	C3DPoint vectorN; // Вектор, куда "смотрит" камера (N в аббревиатуре UVN)
	C3DPoint vectorV; // Вектор "вверх" у камеры (V в аббревиатуре UVN)
	C3DPoint vectorU; // Вектор "вправо" (есть векторное произведение N x V) (N в аббревиатуре UVN)
	CMatrix44 transformationMatrix; // Матрица преобразования данной камеры

	void updateVectorU(); // Обновляет вектор U при изменении любого из векторов N или V (так как это является векторным произведением)
	void updateVectorV(); // Аналогичная операция для вектора V
	void updateTransformationMatrix(); // Обновляет матрицу преобразования при изменении одного из компонент
};

