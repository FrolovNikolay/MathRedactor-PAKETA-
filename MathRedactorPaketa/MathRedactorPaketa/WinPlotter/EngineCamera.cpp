#include "EngineCamera.h"
#include <cmath>
#include <set>

// Ближняя и дальная плоскости отсечения (координаты по Z)
const double CEngineCamera::NearZ = 1;
const double CEngineCamera::FarZ = 100;

CEngineCamera::CEngineCamera(int clientWidth, int clientHeight) :
ViewDistance(1), ClientHeight(clientHeight), ClientWidth(clientWidth), stepSize(1)
{
	// Задаём начальное положение камеры
	Position = C3DPoint(5, 0, 5);
	ViewDirection = C3DPoint(1, 1, 1);
	UpVector = C3DPoint(0, 0, 1);

	UpdateTransformMatrix();
}

void CEngineCamera::UpdateRightVector() {
	RightVector = UpVector.cross(ViewDirection);
}

void CEngineCamera::UpdateTransformMatrix() {
	UpdateRightVector();

	TransformMatrix.Set(0, 0, RightVector.X);
	TransformMatrix.Set(0, 1, UpVector.X);
	TransformMatrix.Set(0, 2, ViewDirection.X);
	TransformMatrix.Set(0, 3, 0);

	TransformMatrix.Set(1, 0, RightVector.Y);
	TransformMatrix.Set(1, 1, UpVector.Y);
	TransformMatrix.Set(1, 2, ViewDirection.Y);
	TransformMatrix.Set(1, 3, 0);

	TransformMatrix.Set(2, 0, RightVector.Z);
	TransformMatrix.Set(2, 1, UpVector.Z);
	TransformMatrix.Set(2, 2, ViewDirection.Z);
	TransformMatrix.Set(2, 3, 0);

	TransformMatrix.Set(3, 0, -Position.dot(RightVector));
	TransformMatrix.Set(3, 1, -Position.dot(UpVector));
	TransformMatrix.Set(3, 2, -Position.dot(ViewDirection));
	TransformMatrix.Set(3, 3, 1);
}

void CEngineCamera::Render(const C3DModel& object, C2DModel& renderedObject) {
	transform(object);
	filter();
	render(renderedObject);
}

void CEngineCamera::SetWindowSize(int clientWidth_, int clientHeight_) {
	if (clientWidth_ <= 0 || clientHeight_ <= 0) {
		throw CEngineCamera::IncorrectWindowSize();
	}
	ClientWidth = clientWidth_;
	ClientHeight = clientHeight_;

	// Расстояние до экрана проекции устанавлиается как половина ширины экрана
	ViewDistance = (ClientWidth - 1) / 2;

	// Соотношение между шириной и высотой
	AspectRatio = ClientWidth / ClientHeight;
}

void CEngineCamera::transform(const C3DModel& object) {
	// Копируем исходный объект
	cameraModel = C3DModel(object);

	// Пробегаемся по всем его точкам и модифицируем при помощи матрицы преобразований
	for (auto point = cameraModel.Points.begin(); point != cameraModel.Points.end(); point++) {
		*point = modifyPoint(*point);
	}
}

void CEngineCamera::filter() {
	// TODO: удаление (и модификация) элементов внутренней структуры

	// В первом приближении мы просто будем удалять те отрезки и треугольники, у которых хотя бы одна вершина попадает
	// вне области видимости камеры 
	std::set<int> pointNumbersForErase; // индексы удаляемых точек
	int counter = 0;
	for (auto point = cameraModel.Points.begin(); point != cameraModel.Points.end(); point++) {
		// Если точка находится вне области видимости
		if (point->Z < NearZ || point->Z > FarZ || std::abs(point->X) > point->Z) {
			pointNumbersForErase.insert(counter);
		}
		counter += 1;
	}

	// Теперь мы удаляем все объекты (индексы), которые содержат хотя бы одну точку из отсечённых
	auto segment = cameraModel.Segments.begin();
	while (segment != cameraModel.Segments.end()) {
		// Если попадает под условие (один из концов находится среди удаляемых точек), то удаляем
		if (pointNumbersForErase.find(segment->First) != pointNumbersForErase.end() &&
			pointNumbersForErase.find(segment->Second) != pointNumbersForErase.end()) {
			segment = cameraModel.Segments.erase(segment);
		}
		// Иначе переходим к следующему элементу
		else {
			segment++;
		}
	}

	// Аналогично и для треугольников
	auto triangle = cameraModel.Triangles.begin();
	while (triangle != cameraModel.Triangles.end()) {
		// Если попадает под условие (одна из вершин треугольника попадает в точки), то удаляем
		if (pointNumbersForErase.find(triangle->First) != pointNumbersForErase.end() &&
			pointNumbersForErase.find(triangle->Second) != pointNumbersForErase.end() &&
			pointNumbersForErase.find(triangle->Third) != pointNumbersForErase.end()) {
			triangle = cameraModel.Triangles.erase(triangle);
		}
		// Иначе переходим к следующему элементу
		else {
			triangle++;
		}
	}
}

void CEngineCamera::render(C2DModel& renderedObject) {
	// TODO: преобразование трёхмерных координат точек в контексте камеры в двухмерные координаты в контексте окна
	renderedObject.Clear();

	// Так как сама структура объекта (отрезки и треугольники) уже не поменяется, то мы просто копируем имеющиеся индексы
	renderedObject.Triangles = cameraModel.Triangles;
	renderedObject.Segments = cameraModel.Segments;

	// Для каждой точки выполняем её аксонометрическое преобразование (то есть проецируем на плоскость обзора камеры)
	for (auto point = cameraModel.Points.begin(); point != cameraModel.Points.end(); point++) {
		C2DPoint newPoint;
		newPoint.X = ViewDistance * point->X / point->Z + (0.5 * ClientWidth - 0.5);
		newPoint.Y = - ViewDistance * point->Y * AspectRatio / point->Z + (0.5 * ClientHeight - 0.5);
		renderedObject.AddPoint(newPoint); 
	}
}

C3DPoint CEngineCamera::modifyPoint(C3DPoint originPoint) const {
	return TransformMatrix.ProjectPoint(originPoint);
}

void CEngineCamera::SetPosition(C3DPoint point) {
	Position = point;
	UpdateTransformMatrix();
}

void CEngineCamera::SetViewDirection(C3DPoint viewDirection_) {
	ViewDirection = viewDirection_.normalize();
	UpdateTransformMatrix();
}

void CEngineCamera::SetViewPoint(C3DPoint viewPoint) {
	SetViewDirection(viewPoint - Position);
}

void CEngineCamera::SetUpVector(C3DPoint upVector_) {
	UpVector = upVector_.normalize();
	UpdateTransformMatrix();
}

void CEngineCamera::MoveForward(double speed) {
	// Вычисляем вектор движения
	C3DPoint movementVector = ViewDirection - Position;
	
	// Прибавляем приращение
	Position = Position + movementVector * speed;

	// Обновляем матрицу преобразования
	UpdateTransformMatrix();
}

void CEngineCamera::MoveUp(double speed) {
	// Прибавляем приращение
	Position = Position + UpVector * speed;

	// Обновляем матрицу преобразования
	UpdateTransformMatrix();
}

void CEngineCamera::MoveSide(double speed) {
	// Прибавляем приращение
	Position = Position + RightVector * speed;

	// Обновляем матрицу преобразования
	UpdateTransformMatrix();
}

void CEngineCamera::rotate(double angle, double x, double y, double z) {
	// Так как при вращении мы изменяем точку направления камеры, то заводим временную переменную
	C3DPoint newViewDirection;

	// Вектор движения
	C3DPoint movementVector = ViewDirection - Position;

	// Синус и косинус вращаемого угла
	double cosTheta = std::cos(angle);
	double sinTheta = std::sin(angle);

	// Найдем значение X точки вращения
	newViewDirection.X = (cosTheta + (1 - cosTheta) * x * x) * movementVector.X;
	newViewDirection.X += ((1 - cosTheta) * x * y - z * sinTheta) * movementVector.Y;
	newViewDirection.X += ((1 - cosTheta) * x * z + y * sinTheta) * movementVector.Z;

	// И значение Y
	newViewDirection.Y = ((1 - cosTheta) * x * y + z * sinTheta) * movementVector.X;
	newViewDirection.Y += (cosTheta + (1 - cosTheta) * y * y) * movementVector.Y;
	newViewDirection.Y += ((1 - cosTheta) * y * z - x * sinTheta) * movementVector.Z;

	// И Z...
	newViewDirection.Z = ((1 - cosTheta) * x * z - y * sinTheta) * movementVector.X;
	newViewDirection.Z += ((1 - cosTheta) * y * z + x * sinTheta) * movementVector.Y;
	newViewDirection.Z += (cosTheta + (1 - cosTheta) * z * z) * movementVector.Z;

	// Теперь просто прибавим новый вектор к нашей позиции,
	// чтобы установить новую позицию камеры.
	ViewDirection = Position + newViewDirection;

	// Обновляем матрицу преобразования
	UpdateTransformMatrix();
}

void CEngineCamera::rotateAroundPoint(C3DPoint point, double angle, double x, double y, double z) {
	// Так как при вращении мы изменяем точку направления камеры, то заводим временную переменную
	C3DPoint newViewDirection;

	// Центр, вокруг которого нужно вращаться
	C3DPoint centerPoint = Position - point;

	// Синус и косинус вращаемого угла
	double cosTheta = std::cos(angle);
	double sinTheta = std::sin(angle);

	// Найдем значение X точки вращения
	newViewDirection.X = (cosTheta + (1 - cosTheta) * x * x) * centerPoint.X;
	newViewDirection.X += ((1 - cosTheta) * x * y - z * sinTheta) * centerPoint.Y;
	newViewDirection.X += ((1 - cosTheta) * x * z + y * sinTheta) * centerPoint.Z;

	// И значение Y
	newViewDirection.Y = ((1 - cosTheta) * x * y + z * sinTheta) * centerPoint.X;
	newViewDirection.Y += (cosTheta + (1 - cosTheta) * y * y) * centerPoint.Y;
	newViewDirection.Y += ((1 - cosTheta) * y * z - x * sinTheta) * centerPoint.Z;

	// И Z...
	newViewDirection.Z = ((1 - cosTheta) * x * z - y * sinTheta) * centerPoint.X;
	newViewDirection.Z += ((1 - cosTheta) * y * z + x * sinTheta) * centerPoint.Y;
	newViewDirection.Z += (cosTheta + (1 - cosTheta) * z * z) * centerPoint.Z;

	// Теперь просто прибавим новый вектор к нашей позиции,
	// чтобы установить новую позицию камеры.
	ViewDirection = point + newViewDirection;

	// Обновляем матрицу преобразования
	UpdateTransformMatrix();
}

// Вращение относительно оси OX
void CEngineCamera::RotateByX(double angle) {
	rotate(angle, 1, 0, 0);
}

// Вращение относительно оси OY
void CEngineCamera::RotateByY(double angle) {
	rotate(angle, 0, 1, 0);
}

// Вращение относительно оси OZ
void CEngineCamera::RotateByZ(double angle) {
	rotate(angle, 0, 0, 1);
}

// Вращение относительно оси OX вокруг заданной точки
void CEngineCamera::RotateAroundPointByX(C3DPoint centerPoint, double angle) {
	rotateAroundPoint(centerPoint, angle, 1, 0, 0);
}

// Вращение относительно оси OY вокруг заданной точки
void CEngineCamera::RotateAroundPointByY(C3DPoint centerPoint, double angle) {
	rotateAroundPoint(centerPoint, angle, 0, 1, 0);
}

// Вращение относительно оси OZ вокруг заданной точки
void CEngineCamera::RotateAroundPointByZ(C3DPoint centerPoint, double angle) {
	rotateAroundPoint(centerPoint, angle, 0, 0, 1);
}

void CEngineCamera::RotateSide(double angle) {
	// rotateAroundPoint(Position, angle, UpVector.X, UpVector.Y, UpVector.Z);
	rotate(angle, UpVector.X, UpVector.Y, UpVector.Z);
}

void CEngineCamera::RotateUp(double angle) {
	rotate(angle, RightVector.X, RightVector.Y, RightVector.Z);
}