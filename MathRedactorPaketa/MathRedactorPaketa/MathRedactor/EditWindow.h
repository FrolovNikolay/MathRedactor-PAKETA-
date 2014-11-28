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

// Поддерживаемые виды функций.
enum TFunctionType {
	FT_YfromX,
	FT_ZfromXY,
	FT_XYfromT,
	FT_XYZfromT,
	FT_XYZfromTL,
	FT_Undefined		// Используется в начальный момент времени.
};

class CEditWindow {
public:
	// Возможные направления движения каретки.
	enum TCaretDirection {
		CD_Up,
		CD_Down,
		CD_Left,
		CD_Right
	};

	CEditWindow();

	// Регистрирует класс окна.
	static bool RegisterClass( HINSTANCE );
	// Создать окно.
	HWND Create( HWND, HINSTANCE );
	// Отобразить окно.
	void Show( int );

	// Отображает каретку.
	void ShowCaret();
	// Скрывает каретку.
	void HideCaret();
	// Двигает каретку на шаг по направлению.
	void MoveCaret( TCaretDirection );

	// Получить высоту простых символов.
	int GetSimpleSymbolHeight() const { return simpleSymbolHeight; }
	// Получение линии, в которой находится каретка.
	CLineOfSymbols* GetCaretLine() { return const_cast<CLineOfSymbols*>( caret.GetLine() ); }
	
	// Добавляет символ и уведомляет окно о том, что надо перерисоваться.
	void AddSymbol( CSymbol* );
	// Добавляет знак с клавиатуры.
	void AddSign( wchar_t );
	// Удаляет символ по текущему положению каретки.
	void RemoveSign();
	// Переводит строчку.
	void NewLine();

	// Получить содержимое редактора в формате для плоттера.
	std::string CalculateStringForPlotter() const;
	// Получить содержимое редактора в Latex формате.
	std::string CalculateLatexString() const;

	// Установка нового типа обрабатываемой функции.
	void SetFunctionType( TFunctionType );
	// Реакция на кнопку по осуществлению проверки на валидность.
	void CheckValidity() const;
	// Экспортировать выделенную часть ввода.
	void ExportSelected() const;

	// Отправляет родительскому окну сообщение о закрытии
	void SendClose() const { ::SendMessage( parentHandle, WM_CLOSE, 0, 0 ); }
	// Передать плоттеру содержимое и сообщить родительскому окну о закрытии.
	void SendAccept() const;

protected:
	// Метод, вызываемый при получении окном сообщения WM_DESTROY.
	void OnWmDestroy();
	// Метод, вызываемый при получении окном сообщения WM_PAINT.
	void OnWmPaint();

	// Обработка изменения размера окна. пересчитывает некоторые свойства скроллов.
	void OnWmSize( LPARAM );
	// Обработка сообщений о скроллировании.
	void OnWmHScroll( WPARAM, LPARAM );
	void OnWmVScroll( WPARAM, LPARAM );

	// Метод, вызываемый при получении окном сообщения WM_LBUTTONDOWN.
	void OnLButDown( LPARAM );
	// Метод, вызываемый при получении окном сообщения WM_LBUTTONUP.
	void OnLButUp( LPARAM );
	// Метод, вызываемый при получении окном сообщения WM_MOUSEMOVE.
	void OnLockedMouseMove( LPARAM );

private:
	// Класс каретки для этого типа окна.
	class CCaret {
	public:
		CCaret( CEditWindow* , const CLineOfSymbols* baseLine );

		void Create();
		void Destroy();

		const CLineOfSymbols* GetLine() const { return caretPosition.CurrentLine; }
		int GetIndex() const { return caretPosition.Index; }

		// Отображается ли в данный момент каретка.
		bool IsShown() const { return isShown; }

		// Показать каретку.
		void Show();
		// Скрыть каретку.
		void Hide();
		
		// Сдвигает каретку на единицу в данном направлении.
		void Move( TCaretDirection );
		// Сдвигает каретку в определенную позицию.
		void MoveTo( const CSymbolPosition& );
		// Переместить каретку в соответствии с координатами в окне.
		void MoveTo( int x, int y );
	private:
		// Окно, которому принадлежит каретка.
		CEditWindow* window;

		// Положение каретки.
		CSymbolPosition caretPosition;

		// Текущие размеры каретки.
		int width;
		int height;

		// Отображается ли каретка.
		bool isShown;

		// Изменить высоту каретки.
		void changeHeight( int );

		// Передвижения каретки для каждого из 4-ех направлений.
		void moveLeft();
		void moveRight();
		void moveUp();
		void moveDown();

		// Сместить каретку к заданной в ней позиции.
		void moveToNewCoordinates();
	};

	// Хэндл окна, которому соответствует этот объект класса.
	HWND windowHandle;

	// Хэндл родителя.
	HWND parentHandle;
	// Имя класса окна.
	static const wchar_t* className;
	
	// Высота простых символов в окне.
	const int simpleSymbolHeight;

	// Содержимое редактора (массив строк).
	std::vector<CLineOfSymbols> content;

	// Константы, связанные со скроллированием.
	const int horizontalScrollUnit;
	const int verticalScrollUnit;

	// Механизм, отвечающий за поиск символа по координате.
	CPositionFinder finder;
	// Механизм, отвечающий за выделение.
	CItemSelector symbolSelector;
	// Механизм, отвечающий за отрисовку окна.
	CEditWindowDrawer drawer;

	// Текущий заданный тип функции.
	TFunctionType currentFunctionType;
	// Набор переменных, от которых определна функция.
	std::set<std::string> knownVariables;
	// До этой позиции в линиях находятся недоступные для редактирования пользователем данные.
	int firstEnablePosition;

	// Каретка.
	CCaret caret;

	// Удалить выделенные символы редактора.
	void removeSelectedItems();

	// Удалить символы, заданные позициями, в одной строке.
	void removeLocalSelected( const CSymbolPosition&, const CSymbolPosition& );

	// Удалить символы, заданные позициями, из редактора.
	void removeGlobalSelected( const CSymbolPosition&, const CSymbolPosition& );

	// Проверка валидности данных введеных в окне. В случае ошибки описание передается через ссылку.
	bool isInputValid( std::wstring& error ) const;

	// Допустим ли данный символ на ввод.
	bool isSymbolAllowed( wchar_t ) const;

	// Получить номер строки в массиве. Возвращается -1 для строк, не являющися базовыми.
	int getBaseLineIndex( const CLineOfSymbols* ) const;

	// Пересчет параметров, связанных со скролированием.
	void recalculateVertScrollParams() const;
	void recalculateHorzScrollParams() const;

	// Установить известные переменные в соответствии с типом функции.
	void setVariables();

	// Установить первую доступную позицию в строке.
	void setFirstEnablePosition();

	// Установка базового ввода.
	void setBaseContent();

	// Вставка в массив строк явных описаний функций.
	void insertOneParametrFunc( wchar_t , wchar_t );
	void insertTwoParametrFunc( wchar_t, wchar_t, wchar_t );

	// Запись в файл выделенных символов.
	bool writeSelectedInFile( LPWSTR fileName, int fileExtentionPos ) const;

	// Стандартная процедура обработки сообщений окном.
	static LRESULT __stdcall windowProcedure( HWND, UINT, WPARAM, LPARAM );
};