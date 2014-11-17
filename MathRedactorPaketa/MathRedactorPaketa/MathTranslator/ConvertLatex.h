// Автор: Азат Давлетшин
// Описание: класс конвертирует дерево MathObj* в строку Latex'а и обратно.

#pragma once

#include "LatexLexerStates.h"
#include "LatexParser.h"
#include "MathFormObj.h"
#include <string>

class CConvertLatex {
public:
	shared_ptr<MathObj> ConvertFromLatex( const std::string& input );
	void ConvertToLatex( shared_ptr<MathObj> moTree, std::string& output ) const;

private:
	void writeNode( shared_ptr<MathObj> node, std::string& output ) const;
	// Проверяет приоритеты, ставит скобки в случае необходимости
	void writeChild( shared_ptr<MathObj> parent, shared_ptr<MathObj> child, std::string& output ) const;
	// Вывести бинарный оператор в строку (за исключением деления)
	void writeBinop( shared_ptr<FormulaObj> node, std::string& output ) const;
	void writeDiv( shared_ptr<FormulaObj> node, std::string& output ) const;
	void writePow( shared_ptr<FormulaObj> node, std::string& output ) const;
	void writeSumOrProd( shared_ptr<FormulaObj> node, std::string& output ) const;
	void writeTrigon( shared_ptr<FormulaObj> node, std::string& output ) const;
	void writeUMinus( shared_ptr<FormulaObj> node, std::string& output ) const;
	void writeRoot(shared_ptr<FormulaObj> node, std::string& output) const;

	// Функция priority из MathML.cpp не подходит, т.к. в этом формате другие приоритеты
	static int priority( shared_ptr<MathObj> node );

	CLatexParser parser;
};