#include "EngineCamera.h"
#include <cmath>
#include <set>


namespace EngineCamera
{
	double Determinant( const C3DPoint& first, const C3DPoint& second, const C3DPoint& third )
	{
		double det = first.X * ( second.Y * third.Z - second.Z * third.Y );
		det -= second.X * ( first.Y * third.Z - first.Z * third.Y );
		det += third.X * ( first.Y * second.Z - first.Z * second.Y );
		return det;
	}

	bool IsCoveredByTriangle( const C3DPoint& first, const C3DPoint& second, const C3DPoint& third, const C3DPoint& looking )
	{
		double x, y, z;
		double det = Determinant( first, second, third );
		if( det == 0. ) {
			return false;
		}

		x = Determinant( looking, second, third ) / det;
		y = Determinant( first, looking, third ) / det;
		z = Determinant( first, second, looking ) / det;

		return x > 0. && y > 0. && z > 0. && x + y + z > 1.;
	}
}

// Ближняя и дальная плоскости отсечения (координаты по Z)
const double CEngineCamera::NearZ = 1;
const double CEngineCamera::FarZ = 100;

CEngineCamera::CEngineCamera( int clientWidth, int clientHeight ) :
ViewDistance( 1 ), ClientHeight( clientHeight ), ClientWidth( clientWidth ), stepSize( 1 )
{
	// устанваливаем движок в начальное положение 
	Reset();
}

void CEngineCamera::Reset()
{
	// Задаём начальное положение камеры
	Position = C3DPoint( 0, 20, 0 );
	ViewDirection = C3DPoint( 0, -1, 0 );
	UpVector = C3DPoint( 0, 0, 1 );
	CenterPoint = C3DPoint( 0, 0, 0 );

	UpdateRightVector();
	UpdateTransformMatrix();

	// своеобразная калибровка
	RotateSideAroundCenter( 0 );
	MoveSide( 0 );
	RotateUpAroundCenter( 0 );
	MoveUp( 0 );
}

void CEngineCamera::UpdateRightVector()
{
	RightVector = UpVector.cross( ViewDirection ).normalize();
}

void CEngineCamera::UpdateUpVector()
{
	UpVector = RightVector.cross( ViewDirection ).normalize() * ( -1 );
}

void CEngineCamera::UpdateTransformMatrix()
{
	TransformMatrix.Set( 0, 0, RightVector.X );
	TransformMatrix.Set( 0, 1, UpVector.X );
	TransformMatrix.Set( 0, 2, ViewDirection.X );
	TransformMatrix.Set( 0, 3, 0 );

	TransformMatrix.Set( 1, 0, RightVector.Y );
	TransformMatrix.Set( 1, 1, UpVector.Y );
	TransformMatrix.Set( 1, 2, ViewDirection.Y );
	TransformMatrix.Set( 1, 3, 0 );

	TransformMatrix.Set( 2, 0, RightVector.Z );
	TransformMatrix.Set( 2, 1, UpVector.Z );
	TransformMatrix.Set( 2, 2, ViewDirection.Z );
	TransformMatrix.Set( 2, 3, 0 );

	TransformMatrix.Set( 3, 0, -Position.dot( RightVector ) );
	TransformMatrix.Set( 3, 1, -Position.dot( UpVector ) );
	TransformMatrix.Set( 3, 2, -Position.dot( ViewDirection ) );
	TransformMatrix.Set( 3, 3, 1 );
}

void CEngineCamera::Render( const C3DModel& object, C2DModel& renderedObject, bool filtrate )
{
	transform( object );
	if( filtrate == true ) {
		filter();
	}
	render( renderedObject );
}

void CEngineCamera::SetWindowSize( int clientWidth_, int clientHeight_ )
{
	if( clientWidth_ <= 0 || clientHeight_ <= 0 ) {
		throw CEngineCamera::IncorrectWindowSize();
	}
	ClientWidth = clientWidth_;
	ClientHeight = clientHeight_;

	// Расстояние до экрана проекции устанавлиается как половина ширины экрана
	ViewDistance = ( ClientWidth - 1 ) / 2;

	// Соотношение между шириной и высотой
	AspectRatio = ClientWidth / ClientHeight;
}

void CEngineCamera::transform( const C3DModel& object )
{
	// Копируем исходный объект
	cameraModel = object;

	// Пробегаемся по всем его точкам и модифицируем при помощи матрицы преобразований
	for( auto point = cameraModel.Points.begin(); point != cameraModel.Points.end(); point++ ) {
		*point = modifyPoint( *point );
	}
}

void CEngineCamera::filter()
{
	std::vector<bool> pointNumbersForErase( cameraModel.Points.size(), false ); // индексы удаляемых точек

	// проверяем, лежит ли точка за треугольником
	for( auto triangle = cameraModel.Triangles.begin(); triangle != cameraModel.Triangles.end(); triangle++ ) {
		int counter = 0;
		for( auto point = cameraModel.Points.begin(); point != cameraModel.Points.end(); point++ ) {
			C3DPoint first = cameraModel.Points[triangle->First];
			C3DPoint second = cameraModel.Points[triangle->Second];
			C3DPoint third = cameraModel.Points[triangle->Third];
			// Если точка находится вне области видимости
			if( EngineCamera::IsCoveredByTriangle( first, second, third, *point ) ) {
				pointNumbersForErase[counter] = true;
			}
			counter += 1;
		}
	}

	// В первом приближении мы просто будем удалять те отрезки и треугольники, у которых хотя бы одна вершина попадает
	// вне области видимости камеры 
	
	int counter = 0;
	for( auto point = cameraModel.Points.begin(); point != cameraModel.Points.end(); point++ ) {
		// Если точка находится вне области видимости
		if( point->Z < NearZ || point->Z > FarZ || std::abs( point->X ) > point->Z ) {
			pointNumbersForErase[ counter ] = true;
		}
		counter += 1;
	}

	// Теперь мы удаляем все объекты (индексы), которые содержат хотя бы одну точку из отсечённых
	auto segment = cameraModel.Segments.begin();
	while( segment != cameraModel.Segments.end() ) {
		// Если попадает под условие (оба конца находятся среди удаляемых точек), то удаляем
		if (pointNumbersForErase[segment->First] ||	pointNumbersForErase[segment->Second] ) {
			segment = cameraModel.Segments.erase( segment );
		}
		// Иначе переходим к следующему элементу
		else {
			segment++;
		}
	}

	// Аналогично и для треугольников
	auto triangle = cameraModel.Triangles.begin();
	while( triangle != cameraModel.Triangles.end() ) {
		int check = 0;
		if( pointNumbersForErase[triangle->First] ) check++;
		if( pointNumbersForErase[triangle->Second] ) check++;
		if( pointNumbersForErase[triangle->Third] ) check++;
		// Если попадает под условие (хоть одна вершина треугольника попадает в точки), то удаляем
		if( check > 0 ) {
			triangle = cameraModel.Triangles.erase( triangle );
		}
		// Иначе переходим к следующему элементу
		else {
			triangle++;
		}
	}
}

void CEngineCamera::render( C2DModel& renderedObject ) const
{
	// TODO: преобразование трёхмерных координат точек в контексте камеры в двухмерные координаты в контексте окна
	renderedObject.Clear();

	// Так как сама структура объекта (отрезки и треугольники) уже не поменяется, то мы просто копируем имеющиеся индексы
	renderedObject.Triangles = cameraModel.Triangles;
	renderedObject.Segments = cameraModel.Segments;

	// Для каждой точки выполняем её аксонометрическое преобразование (то есть проецируем на плоскость обзора камеры)
	for( auto point = cameraModel.Points.begin(); point != cameraModel.Points.end(); point++ ) {
		C2DPoint newPoint;
		newPoint.X = ViewDistance * point->X / point->Z + ( 0.5 * ClientWidth - 0.5 );
		newPoint.Y = -ViewDistance * point->Y * AspectRatio / point->Z + ( 0.5 * ClientHeight - 0.5 );
		renderedObject.AddPoint( newPoint );
	}
}

C3DPoint CEngineCamera::modifyPoint( C3DPoint originPoint ) const
{
	return TransformMatrix.ProjectPoint( originPoint );
}

void CEngineCamera::SetPosition( const C3DPoint& point )
{
	Position = point;
	UpdateRightVector();
	UpdateTransformMatrix();
}

void CEngineCamera::SetViewDirection( const C3DPoint& viewDirection_ )
{
	ViewDirection = viewDirection_.normalize();
	UpdateTransformMatrix();
}

void CEngineCamera::SetViewPoint( const C3DPoint& viewPoint )
{
	SetViewDirection( viewPoint - Position );
}

void CEngineCamera::SetUpVector( const C3DPoint& upVector_ )
{
	UpVector = upVector_.normalize();
	UpdateRightVector();
	UpdateTransformMatrix();
}

void CEngineCamera::MoveForward( double speed )
{
	// Вычисляем вектор движения
	C3DPoint movementVector = ViewDirection - Position;

	// Прибавляем приращение
	Position = Position + movementVector * speed;

	// Обновляем матрицу преобразования
	UpdateTransformMatrix();
}

void CEngineCamera::MoveUp( double speed )
{
	// Прибавляем приращение
	Position = Position + UpVector * speed;
	CenterPoint = CenterPoint + UpVector * speed;

	// Обновляем матрицу преобразования
	UpdateTransformMatrix();
}

void CEngineCamera::MoveSide( double speed )
{
	// Прибавляем приращение
	Position = Position + RightVector * speed;
	CenterPoint = CenterPoint + RightVector * speed;

	// Обновляем матрицу преобразования
	UpdateTransformMatrix();
}

void CEngineCamera::RotateSideAroundCenter( double angle )
{
	// Так как при вращении мы изменяем точку направления камеры, то заводим временную переменную
	C3DPoint newViewPosition;


	// Синус и косинус вращаемого угла
	double cosTheta = std::cos( angle );
	double sinTheta = std::sin( angle );
	C3DPoint os = Position - CenterPoint;

	double x = UpVector.X;
	double y = UpVector.Y;
	double z = UpVector.Z;

	// Найдем значение X точки вращения
	newViewPosition.X = ( cosTheta + ( 1 - cosTheta ) * x * x ) * os.X;
	newViewPosition.X += ( ( 1 - cosTheta ) * x * y - z * sinTheta ) * os.Y;
	newViewPosition.X += ( ( 1 - cosTheta ) * x * z + y * sinTheta ) * os.Z;

	// И значение Y
	newViewPosition.Y = ( ( 1 - cosTheta ) * x * y + z * sinTheta ) * os.X;
	newViewPosition.Y += ( cosTheta + ( 1 - cosTheta ) * y * y ) * os.Y;
	newViewPosition.Y += ( ( 1 - cosTheta ) * y * z - x * sinTheta ) * os.Z;

	// И Z...
	newViewPosition.Z = ( ( 1 - cosTheta ) * x * z - y * sinTheta ) * os.X;
	newViewPosition.Z += ( ( 1 - cosTheta ) * y * z + x * sinTheta ) * os.Y;
	newViewPosition.Z += ( cosTheta + ( 1 - cosTheta ) * z * z ) * os.Z;

	// Теперь просто прибавим новый вектор к нашей позиции,
	// чтобы установить новую позицию камеры.
	Position = newViewPosition + CenterPoint;
	SetViewPoint( CenterPoint );

	// Обновляем матрицу преобразования
	UpdateRightVector();
	UpdateTransformMatrix();
}

void CEngineCamera::RotateUpAroundCenter( double angle )
{
	// Так как при вращении мы изменяем точку направления камеры, то заводим временную переменную
	C3DPoint newViewPosition;


	// Синус и косинус вращаемого угла
	double cosTheta = std::cos( angle );
	double sinTheta = std::sin( angle );
	C3DPoint os = Position - CenterPoint;

	double x = RightVector.X;
	double y = RightVector.Y;
	double z = RightVector.Z;

	// Найдем значение X точки вращения
	newViewPosition.X = ( cosTheta + ( 1 - cosTheta ) * x * x ) * os.X;
	newViewPosition.X += ( ( 1 - cosTheta ) * x * y - z * sinTheta ) * os.Y;
	newViewPosition.X += ( ( 1 - cosTheta ) * x * z + y * sinTheta ) * os.Z;

	// И значение Y
	newViewPosition.Y = ( ( 1 - cosTheta ) * x * y + z * sinTheta ) * os.X;
	newViewPosition.Y += ( cosTheta + ( 1 - cosTheta ) * y * y ) * os.Y;
	newViewPosition.Y += ( ( 1 - cosTheta ) * y * z - x * sinTheta ) * os.Z;

	// И Z...
	newViewPosition.Z = ( ( 1 - cosTheta ) * x * z - y * sinTheta ) * os.X;
	newViewPosition.Z += ( ( 1 - cosTheta ) * y * z + x * sinTheta ) * os.Y;
	newViewPosition.Z += ( cosTheta + ( 1 - cosTheta ) * z * z ) * os.Z;

	// Теперь просто прибавим новый вектор к нашей позиции,
	// чтобы установить новую позицию камеры.
	Position = newViewPosition + CenterPoint;
	SetViewPoint( CenterPoint );

	// Обновляем матрицу преобразования
	UpdateUpVector();
	UpdateTransformMatrix();
}