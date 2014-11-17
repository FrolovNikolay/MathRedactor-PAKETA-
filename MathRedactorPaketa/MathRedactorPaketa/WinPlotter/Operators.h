// Автор: Федюнин Валерий
// Описание: набор операторов, учавствующих в выполнении формулы (узлы дерева разбора формулы)

#pragma once

#include <map>
#include <memory>

#include "Enums.h"

// Интерфейс оператора
class IOperator {
public:
	virtual ~IOperator() {};

	virtual double Calculate( const std::map<char, double>& variables ) const = 0;
private:
};

// Оператор - константа
class CConstant : public IOperator {
public:
	CConstant( double value );

	double Calculate( const std::map<char, double>& variables ) const;

private:
	double value;
};

// Оператор - переменная
class CVariable : public IOperator {
public:
	CVariable( char name );

	double Calculate( const std::map<char, double>& variables ) const;

private:
	// имя переменной, которой соответствует данный узел
	char variableName;
};

// Бинарный оператор
class CBinaryOperator : public IOperator {
public:
	CBinaryOperator( IOperator* left, IOperator* right, BINOP type );

	double Calculate( const std::map<char, double>& variables ) const;

private:
	// выражения слева и справа от оператора
	std::shared_ptr<IOperator> left;
	std::shared_ptr<IOperator> right;
	// тип оператора
	BINOP type;
};

// Оператор - функция
class CFunction : public IOperator {
public:
	CFunction( IOperator* parameter, FUNC type );

	double Calculate( const std::map<char, double>& variables ) const;

private:
	// выражение, подаваемое на вход функции
	std::shared_ptr<IOperator> parameter;
	// тип функции
	FUNC type;
};