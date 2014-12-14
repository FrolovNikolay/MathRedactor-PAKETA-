/*
	author: Timur Khusaenov
*/
#pragma once
#include <sstream>
#include <Windows.h>

namespace WinPlotter
{
	void SetParamText( HWND hWnd, int ID, char c )
	{
		std::wstringstream wss;
		wss << L"Диапазон параметра " << c;
		::SetWindowText( ::GetDlgItem( hWnd, ID ), wss.str().c_str() );
	}

	void SetValue( HWND hWnd, int ID, double d )
	{
		std::wstringstream wss;
		wss << d;
		::SetWindowText( ::GetDlgItem( hWnd, ID ), wss.str().c_str() );
	}

	void HideEdit( HWND hWnd, int ID )
	{
		::ShowWindow( ::GetDlgItem( hWnd, ID ), SW_HIDE );
	}
}