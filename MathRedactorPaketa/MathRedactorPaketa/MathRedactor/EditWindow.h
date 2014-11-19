// Автор: Федюнин Валерий
// Описание: класс окна, в котором отрисовываются и изменяются формулы

#pragma once

#include <Windows.h>
#include <vector>
#include "LineOfSymbols.h"
#include "ItemSelector.h"

class CEditWindow {
public:
	CEditWindow();
	~CEditWindow();

	//Временно
	int GetSimpleSymbolHeight() const { return simpleSymbolHeight; }
	CLineOfSymbols* GetCaretLine() { return caret.GetLine(); }

	// регистрирует класс окна
	static bool RegisterClass( HINSTANCE );

	// Создает окно
	HWND Create( HWND, HINSTANCE );
	// Отображается окно
	void Show( int );
	
	// Добавляет символ и уведомляет окно о том, что надо перерисоваться
	void AddSymbol( CSymbol* );
	// Добавляет знак с клавиатуры
	void AddSign( wchar_t );
	// Удаляет символ (например, при нажатии Backspace)
	void RemoveSign();
	// Переводит строчку
	void NewLine();
	
	// возможные направления движения каретки
	enum TDirection {
		DUp, DDown, DLeft, DRight
	};
	// Отображает каретку
	void ShowCaret();
	// Скрывает каретку
	void HideCaret();
	// Двигает каретку на шаг по направлению
	void MoveCaret( TDirection );
	// Двигает каретку к данной точке
	void MoveCaretTo( int x, int y );


protected:
	// метод, вызываемый при получении окном сообщения WM_DESTROY
	void OnWmDestroy();
	// метод, вызываемый при получении окном сообщения WM_PAINT
	void OnWmPaint();
	// скроллирование окон
	void OnWmHScroll( WPARAM, LPARAM );
	void OnWmVScroll( WPARAM, LPARAM );
	// обработка изменения размера окна
	// пересчитывает некоторые свойства скроллов
	void OnWmSize( LPARAM );
	// метод, вызываемый при получении окном сообщения WM_LBUTTONDOWN
	void OnLButDown( LPARAM );
	// метод, вызываемый при получении окном сообщения WM_LBUTTONUP
	void OnLButUp( LPARAM );
	// метод, вызываемый при получении окном сообщения WM_MOUSEMOVE
	void OnLockedMouseMove( LPARAM );

private:
	// хэндл окна, которому соответствует этот объект класса.
	HWND windowHandle;
	// имя класса окна
	static const wchar_t* className;

	//Содержимое редактора (массив строк)
	std::vector<CLineOfSymbols> content;
	
	//TODO: Сделать нормальные настройки
	int simpleSymbolHeight;

	// для скроллирования
	const int horizontalScrollUnit;
	const int verticalScrollUnit;

	// связанный с окном механизм для выделения
	CItemSelector symbolSelector;
	
	// Класс каретки для этого типа окна
	class CCaret {
	public:
		CCaret( CEditWindow* );

		CLineOfSymbols* GetLine();
		int GetIndex() const;

		// видит ли пользователь каретку в данный момент
		bool IsShown() const;

		void Create();
		void Destroy();

		// показывает/скрывает каретку
		void Show();
		void Hide();
		
		// сдвигает каретку на единицу в данном направлении
		void Move( TDirection );
		// сдвигает каретку в определенную позицию
		void MoveTo( CLineOfSymbols*, int );

	private:
		// окно, которому принадлежит каретка
		CEditWindow* window;
		// текущий размер каретки
		int width;
		int height;
		// положение каретки
		// линия, в которой находится каретка
		CLineOfSymbols* line;
		// индекс символа, перед которым стоит каретка
		// если равен длине строки - каретка стоит в конце строки
		int index;
		// отображается ли каретка в данный момент
		bool shown;

		void moveLeft();
		void moveRight();
		void moveUp();
		void moveDown();

		void moveToNewCoordinates();
		void changeHeight( int );
	};

	// каретка
	CCaret caret;

	static LRESULT __stdcall windowProcedure( HWND, UINT, WPARAM, LPARAM );

	CLineOfSymbols* isLineBase( CLineOfSymbols* currentBaseLine, int x, int y );

	void removeSelectedSymbols();

	bool isSymbolAllowed( wchar_t ) const;
	int getBaseLineIndex( const CLineOfSymbols* ) const;

	void recalculateVertScrollParams() const;
	void recalculateHorzScrollParams() const;
};