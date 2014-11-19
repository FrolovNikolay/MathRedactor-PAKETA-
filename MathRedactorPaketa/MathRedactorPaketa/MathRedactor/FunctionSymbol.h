// �����: ��������� ���������
// ��������: ����� ������������ ����� ������� � ����� ����������
// � ��� ����� �����, ������� � ��

#pragma once

#include "Symbol.h"
#include "LineOfSymbols.h" 


class CFunctionSymbol : public CSymbol {
public:

	CFunctionSymbol( int simpleSymbolHeight, wchar_t* _funcName );

	// �������� ����� (deep) �������. 
	virtual CSymbol* Clone( CLineOfSymbols* parent ) const;
	virtual void UpdateParent( CLineOfSymbols* parent );

	// �������� ������ ���������� �� ��������� (����� ���� ������) 
	virtual void GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const;
	virtual void GetSubstrings( std::vector<CLineOfSymbols*>& substrings );

	virtual int GetHeight( int simpleSymbolHeight ) const;
	virtual int GetBaselineOffset( int simpleSymbolHeight ) const;
	virtual int GetDescent( int simpleSymbolHeight ) const;

	CLineOfSymbols& GetArgumentLine() { return argumentLine; }
	const CLineOfSymbols& GetArgumentLine() const { return argumentLine; }

	// �������� ���� ������� ��������� � Symbol.h
	virtual void Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const;
	virtual int CalculateWidth( HDC displayHandle ) const;

private:

	// �������� ������� ������� ������������ �� ������
	CLineOfSymbols functionName;
	// ������������� ������� ������ ������ ��������� ������������ ��������
	CLineOfSymbols closingBracket;
	// ��������� �������
	CLineOfSymbols argumentLine;

};