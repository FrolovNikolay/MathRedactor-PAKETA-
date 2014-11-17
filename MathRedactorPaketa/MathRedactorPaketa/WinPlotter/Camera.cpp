#include "Camera.h"

CCamera::CCamera()
{
}

// Устанавливает камеру в заданную точку трёхмерного пространства
void CCamera::SetPosition(const C3DPoint& positionPoint) {
	position = positionPoint;

	// Обновляем матрицу, так как изменили положение камеры
	updateTransformationMatrix();
}

void CCamera::MoveForward(double speed) {
	// Прибавляем приращение
	position += vectorN * speed;

	// Обновляем матрицу преобразования
	updateTransformationMatrix();
}

void CCamera::MoveUp( double speed )
{
	// Прибавляем приращение
	position += vectorV * speed;

	// Обновляем матрицу преобразования
	updateTransformationMatrix();
}

void CCamera::MoveSide( double speed )
{
	// Прибавляем приращение
	position += vectorU * speed;

	// Обновляем матрицу преобразования
	updateTransformationMatrix();
}

// Обновляет матрицу преобразования при изменении одного из компонент
void CCamera::updateTransformationMatrix()
{
	// Первая строка
	transformationMatrix.Set( 0, 0, vectorU.X );
	transformationMatrix.Set( 0, 1, vectorV.X );
	transformationMatrix.Set( 0, 2, vectorN.X );
	transformationMatrix.Set( 0, 3, 0 );

	// Вторая строка
	transformationMatrix.Set( 1, 0, vectorU.Y );
	transformationMatrix.Set( 1, 1, vectorV.Y );
	transformationMatrix.Set( 1, 2, vectorN.Y );
	transformationMatrix.Set( 1, 3, 0 );

	// Третья строка
	transformationMatrix.Set( 2, 0, vectorU.Z );
	transformationMatrix.Set( 2, 1, vectorV.Z );
	transformationMatrix.Set( 2, 2, vectorN.Z );
	transformationMatrix.Set( 2, 3, 0 );

	// Четвёртая строка
	transformationMatrix.Set( 3, 0, -position.dot( vectorU ) );
	transformationMatrix.Set( 3, 1, -position.dot( vectorV ) );
	transformationMatrix.Set( 3, 2, -position.dot( vectorN ) );
	transformationMatrix.Set( 3, 3, 1 );
}

void CCamera::updateVectorU()
{
	// Векторное произведение U = V x N
	vectorU = vectorV.cross( vectorN );
}

void CCamera::updateVectorV()
{
	// Векторное произведение V = N x U
	vectorV = vectorN.cross( vectorU );
}