﻿#pragma once
#include "3DPoint.h"
#include "Matrix44.h"
#include "Model.h"

/*
* Класс движка, который переводит трёхмерные объекты пространства графика в двухмерные объекты контекста окна отрисовки.
* Камера описывается в системе UVN.
*/
class CEngineCamera
{
public:
	CEngineCamera(int clientWidth = 640, int clientHeight = 480);

	// Устанавливает камеру в заданную точку трёхмерного пространства
	void SetPosition( const C3DPoint& position);

	// Устанавливает вектор, в направлении которого "смотрит" камера
	void SetViewDirection( const C3DPoint& viewDirection);

	// Устанавливает точку, куда "смотрит" камера
	void SetViewPoint(const C3DPoint& viewPoint);

	// Устанавливает вертикальный вектор камеры (куда "смотрит" вверх камеры)
	void SetUpVector( const C3DPoint& upVector);

	// Двигает камеру вперёд/назад
	void MoveForward(double speed = 1.0f);
	// Двигает камеру вверх/вниз
	void MoveUp(double speed = 1.0f);
	// Двигает камеру влево/вправо
	void MoveSide(double speed = 1.0f);

	// Вращает камеру вверх/вниз и влево/вправо
	void RotateSideAroundCenter( double angle );
	void RotateUpAroundCenter( double angle );

	void RotateSide( double angle );
	void RotateUp( double angle );


	// Устанавливает размеры клиентской области ширины и высоты окна
	void SetWindowSize( int clientWidth, int clientHeight );

	// Ключевой метод, который обрабатывает трёхмерный объект. По ссылке renderedObject записывает двухмерные примитивы для
	// отрисовки в окне программы
	void Render( const C3DModel& object, C2DModel& renderedObject, bool filtrate = true );

	// функция возвращающая движок в начальное состояние
	void Reset();

private:
	// Положение камеры в трёхмерном пространстве
	C3DPoint Position;

	// Направление камеры (вектор N)
	C3DPoint ViewDirection;

	// Вертикальный вектор камеры (вектор V)
	C3DPoint UpVector;

	// Вектор, направленный вправо (он же векторное произведение U = V x N)
	C3DPoint RightVector;

	// центр вращения
	C3DPoint CenterPoint;

	// Обновляет вектор U при изменений первых N или V
	void UpdateRightVector();
	void UpdateUpVector();

	// Матрица преобразования координат из системы XYZ объекта в систему UVN
	CMatrix44 TransformMatrix;
	// Обновляет эту матрицу в случае изменения положения и вращения камеры
	void UpdateTransformMatrix();

	// Размер шага передвижения камеры
	double stepSize;

	// Расстояние обзора
	double ViewDistance;

	// Ближняя и дальная плоскости отсечения (координаты по Z)
	static const double NearZ;
	static const double FarZ;

	// Размер проекции (разрешение окна, куда будет генерироваться картинка) и соотношение между размерами
	int ClientWidth, ClientHeight;
	double AspectRatio;

	// Исключение, вызываемое при неверно переданных параметрах размеров проекции
	class IncorrectWindowSize {};

	// Структура, хранящая координаты модели в пространстве камеры
	C3DModel cameraModel;

	// Внутренний метод, который создаёт локальную версию объекта с координатами камеры
	void transform( const C3DModel& object );
	
	// Фильтрует узлы и элементы модели, которые не попадают в область обзора камеры
	void filter();

	// Проецирует точки внутренней структуры в двухмерную модель для отрисовки на экране
	void render( C2DModel& renderedObject ) const;

	// Возвращает преобразованные координаты точки при помощи матрицы преобразований
	C3DPoint modifyPoint( C3DPoint originPoint ) const;
};

