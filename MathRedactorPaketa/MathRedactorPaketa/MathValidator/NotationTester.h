// Автор: Воротилов Владислав.

// Описание: класс, проверяющий корректность вычислений, заданных деревом разбора,
// полученным от NotationBuilder.

#pragma once

#include <string>
#include <TokenTypes.h>
#include <NotationBuilder.h>
#include <stack>
#include <iostream>
#include <set>

class CNotationTester {
public:
	// Метод тестирования валидности формулы в виде latex-строки.
	bool Test( const std::string& src, const std::set<std::string>& _knownVars );

private:
	// Здесь хранятся все переменные.
	std::set<std::string> knownVars;
	// Используется для работы с переменными сумм.
	std::stack<std::string> sumVars;
	// В этом стеке храним все переменные и числа.
	std::stack<CTexToken> numbersStack;
	// Токены
	std::vector<std::string> tokens;

	// Информация о последнем "вынутом" токене, нужна обработчику ошибок, чтобы понять, где произошло "падение".
	std::pair<std::string, int> currentNotation;

	// Проверка унарной операции(например sin) на корректность, в соответствии с деревом.
	void testUnaryOperation( const CTexToken& operation );

	// Построение нового токена по унарной операции и аргументу в зависимости от их типов.
	CTexToken tokenUnaryOperation( const CTexToken& operation, const CTexToken& arg ) const;

	// Проверка бинарной операции(например * или frac{}{}) на корректность, в соответствии с деревом.
	void testBinaryOperation( const CTexToken& operation );

	// Построение нового токена по бинарной операции и ее аргументам в зависимости от их типов.
	CTexToken tokenBinaryOperation( const CTexToken& operation, const CTexToken& arg1, const CTexToken& arg2 ) const;

	// Построение нового токена по булевой операции и ее аргументам в зависимости от их типов.
	CTexToken tokenBoolOperation( const CTexToken& operation, const CTexToken& arg1, const CTexToken& arg2 );

	// Проверка аггрегирующих операций(например sum) на корректность, в соответствии с деревом.
	void testAggregationOperation( const CTexToken& operation );

	// Проверка присваивания на корректность.
	void testAssignOperation();

	// Обнуление внутренних значений для повторного использования.
	void reinitialize();
};