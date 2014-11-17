#pragma once
#include "Windows.h"
#include "EngineCamera.h"
#include "Model.h"

class CWinPlotter {
public:
	// Трёхмерный примитив, который будет рисоваться на экране
	C3DModel testObject;

	static bool registerClass( HINSTANCE hInstance );
	HWND create( HINSTANCE hInctance, HWND parent );
	void show( int cmdShow );

	void moveX( LONG times = 1 );
	void moveY( LONG times = 1 );
	void rotateX( LONG times = 1 );
	void rotateY( LONG times = 1 );
	void zoom( LONG times = 1 );
	void clear();

protected:
	void OnCreate();
	void OnDestroy();
	void OnSize();
	void Invalidate();
	void PaintObject();

	// Обновляет размеры проекции для движка
	void UpdateScreenSize();

private:
	HWND handle;

	// Экземпляр движка, который будет заниматься рендером объекта
	CEngineCamera engine;
	// Коэффициент, с которым будет приближаться/удаляться объект
	const double engineZoomFactor = 0.03f;
	// Коэффициент, с которым будет вращаться объект
	const double engineRotationFactor = 0.0005f;
	// Коэффициент, с которым будет перемещаться камера
	const double engineMovementFactor = 0.1f;

	// Вспомогательный объект, содержащий оси координат (с направлением)
	C3DModel axisObject;
	// Длина отрисовки осей в каждое из направлений
	const int axisLength = 10;

	// Структура двухмерного объекта, который будет непосредственно переводиться в вызовы WinAPI
	C2DModel renderedObject;
	// Аналог для объекта с осями
	C2DModel axisRenderedObject;

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
};