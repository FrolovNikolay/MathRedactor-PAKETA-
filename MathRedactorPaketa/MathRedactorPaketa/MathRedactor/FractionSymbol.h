//�����: ��������� ����

#pragma once

#include "Symbol.h"
#include "LineOfSymbols.h" 

//������� ������ "�����". ������� �� ���� ����� (LineOfSymbols).
class CFractionSymbol : public CSymbol {
public:

	CFractionSymbol( int simpleSymbolHeight );

	//�������� ����� (deep) �������. 
	virtual CSymbol* Clone( CLineOfSymbols* ) const;
	virtual void UpdateParent( CLineOfSymbols* parent );
	//�������� ������ ���������� �� ��������� (����� ���� ������)
	virtual void GetSubstrings( std::vector<const CLineOfSymbols*>& substrings ) const;
	virtual void GetSubstrings( std::vector<CLineOfSymbols*>& substrings );

	virtual int GetHeight( int simpleSymbolHeight ) const;
	virtual int GetBaselineOffset( int simpleSymbolHeight ) const;
	virtual int GetDescent( int simpleSymbolHeight ) const;

	CLineOfSymbols& GetUpperLine( ) { return upperLine; }
	const CLineOfSymbols& GetUpperLine( ) const { return upperLine; }
	CLineOfSymbols& GetLowerLine( ) { return lowerLine; }
	const CLineOfSymbols& GetLowerLine( ) const { return lowerLine; }

	//�������� ���� ������� ��������� � Symbol.h
	virtual void Draw( HDC displayHandle, int posX, int posY, int simpleSymbolHeight ) const;
	virtual int CalculateWidth( HDC displayHandle ) const;

private:
	//������� � ������ ����� �����
	CLineOfSymbols upperLine;
	CLineOfSymbols lowerLine;

	//������ ���������� ������ ������������ ����� ����� � ������� ����� �����
	int getUpperOffset( ) const { return 3; }
	int getLowerOffset( ) const { return 3; }
	int getStrokeHeight( ) const { return 1; }
	//������� � ������ ����� (����������� � ������ �������)
	int getStrokeAddition( ) const { return 1; }
	int getSymbolBorder( ) const { return 1; }
};