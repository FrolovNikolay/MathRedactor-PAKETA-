// Автор: Федюнин Валерий.

// Описание: класс окна, в котором отрисовываются и изменяются формулы.

#pragma once

#include <Windows.h>
#include <vector>
#include <set>
#include "LineOfSymbols.h"
#include "ItemSelector.h"
#include "PositionFinder.h"
#include "EditWindowDrawer.h"
#include "Messages.h"

enum TFunctionType {
	FT_YfromX,
	FT_ZfromXY,
	FT_XYfromT,
	FT_XYZfromT,
	FT_XYZfromTL
};

class CEditWindow {
public:
	// возможные направления движения каретки
	enum TCaretDirection {
		CD_Up,
		CD_Down,
		CD_Left,
		CD_Right
	};

	CEditWindow();

	// регистрирует класс окна
	static bool RegisterClass( HINSTANCE );

	int GetSimpleSymbolHeight() const { return simpleSymbolHeight; }

	CLineOfSymbols* GetCaretLine() { return const_cast<CLineOfSymbols*>( caret.GetLine() ); }

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
	
	// Отображает каретку
	void ShowCaret();
	// Скрывает каретку
	void HideCaret();
	// Двигает каретку на шаг по направлению
	void MoveCaret( TCaretDirection );

	// Получить содержимое редактора в формате для плоттера.
	std::string CalculateStringForPlotter() const;
	// Получить содержимое редактора в Latex формате.
	std::string CalculateLatexString() const;

	// Установка нового типа обрабатываемой функции.
	void SetFunctionType( TFunctionType );

	// Реакция на кнопку по осуществлению проверки на валидность.
	void CheckValidity() const;

	void ExportSelected() const;

	// отправляет родительскому окну сообщение о закрытии
	void SendClose() const { ::SendMessage( parentHandle, WM_CLOSE, 0, 0 ); }
	void SendAccept() const;

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
	// Класс каретки для этого типа окна
	class CCaret {
	public:
		CCaret( CEditWindow* , const CLineOfSymbols* baseLine );

		const CLineOfSymbols* GetLine() const { return caretPosition.CurrentLine; }
		int GetIndex() const { return caretPosition.Index; }

		// видит ли пользователь каретку в данный момент
		bool IsShown() const { return shown; }

		void Create();
		void Destroy();

		// показывает/скрывает каретку
		void Show();
		void Hide();
		
		// сдвигает каретку на единицу в данном направлении
		void Move( TCaretDirection );
		// сдвигает каретку в определенную позицию
		void MoveTo( const CSymbolPosition& );

		void MoveTo( int x, int y );

	private:
		// окно, которому принадлежит каретка
		CEditWindow* window;
		// положение каретки
		CSymbolPosition caretPosition;
		// текущий размер каретки
		int width;
		int height;
		// отображается ли каретка в данный момент
		bool shown;

		void moveLeft();
		void moveRight();
		void moveUp();
		void moveDown();

		void moveToNewCoordinates();
		void changeHeight( int );
	};
	// хэндл окна, которому соответствует этот объект класса.
	HWND windowHandle;
	// хэндл родителя
	HWND parentHandle;
	// имя класса окна
	static const wchar_t* className;
	
	const int simpleSymbolHeight;

	//Содержимое редактора (массив строк)
	std::vector<CLineOfSymbols> content;

	// для скроллирования
	const int horizontalScrollUnit;
	const int verticalScrollUnit;

	// Отвечает за поиск символа по координате.
	CPositionFinder finder;
	// Отвечает за выделение.
	CItemSelector symbolSelector;
	// Отвечает за прорисовку окна.
	CEditWindowDrawer drawer;

	// Текущий заданный тип функции.
	TFunctionType currentFunctionType;
	// Набор переменных, от которых определна функция.
	std::set<std::string> knownVariables;
	// До этой позиции в линиях находится служебная информация.
	int firstEnablePosition;

	// каретка
	CCaret caret;

	CLineOfSymbols* isLineBase( CLineOfSymbols* currentBaseLine, int x, int y );

	void removeSelectedItems();

	void removeLocalSelected( const CSymbolPosition&, const CSymbolPosition& );

	void removeGlobalSelected( const CSymbolPosition&, const CSymbolPosition& );

	// Проверка валидности данных введеных в окне. В случае ошибки описание передается через ссылку.
	bool isInputValid( std::wstring& error ) const;

	bool isSymbolAllowed( wchar_t ) const;
	int getBaseLineIndex( const CLineOfSymbols* ) const;

	void recalculateVertScrollParams() const;
	void recalculateHorzScrollParams() const;

	// Установить известные переменные в соответствии с типом.
	void setVariables();
	// Установить первую доступную позицию в строке.
	void setFirstEnablePosition();
	// Установка базового ввода.
	void setBaseContent();

	void insertOneParametrFunc( wchar_t , wchar_t );

	void insertTwoParametrFunc( wchar_t, wchar_t, wchar_t );

	bool writeSelectedInFile( LPWSTR fileName, int fileExtentionPos ) const;

	static LRESULT __stdcall windowProcedure( HWND, UINT, WPARAM, LPARAM );
};