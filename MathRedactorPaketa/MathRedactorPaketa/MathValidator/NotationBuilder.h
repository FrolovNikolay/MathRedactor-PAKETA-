// Автор: Николай Фролов

// Описание: Класс совершает построение дерева разбора на основе latex строки.
// Построение происходит в конструкторе, который может выбросить исключение в случае некоторых
// синтаксических/лексических ошибок.

#pragma once
#include <TokenTypes.h>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <stack>

class CNotationBuilder {
public:

	// Принимает на вход формулу в формате latex.
	// В случае некотроых синтаксических/лексических ошибок 
	// генерируется исключение с описанием ошибки.
	explicit CNotationBuilder( const std::string& src );

	// Получить дерево разбора в виде стека, с парами (токен, индекс), где индекс определяет нахождение токена в начальной строке.
	std::stack< std::pair<std::string, int> > GetReverseNotation() const { return outputNotation; }
	// Получить исходный текст в виде последовательности токенов
	std::vector<std::string> GetTokens() const { return tokens; }

private:
	// Регулярные выражения для проверки.
	static const std::regex numberRegex;
	static const std::regex variableRegex;
	static const std::regex rightOpRegex;
	static const std::regex leftOpRegex;
	static const std::regex unaryFunctionRegex;
	static const std::regex unarySpecifiedFunctionRegex;
	static const std::regex binaryFunctionRegex;
	static const std::regex agregationFunctionRegex;


	// Все возможные варианты используемых скобок.
	static const std::string bracket[];
	// Отображение связывающее операторы с их приоритетом.
	static const std::map< std::string, int > operatorsPriority;
	// Размер массива скобок.
	static const int bracArraySize;


	// Флаг указывающий может ли следующая операция быть унарной.
	bool mayUnary;
	// Текущий обрабатываемый токен.
	std::string currentToken;
	// Входной текст, разбитый на приемлимые токены
	std::vector<std::string> tokens;
	// Сюда попадают токены, которые уже встретились в тексте, но еще не могут быть обработаны.
	std::stack< std::pair<std::string, int> > tempStack;
	// Дерево разбора с парами (токен, индекс), где индекс определяет нахождение токена в начальной строке.
	std::stack< std::pair<std::string, int> > outputNotation;

	// Проверяет распределение скобок в выражении на корректность
	void checkBrackets() const;

	// Для открывающей скобки с указанным индексом возвращает индекс соответствующей закрывающей скобки.
	// Если закрывающая скобка не обнаружена возвращает -1.
	int findClosingBracket( int idx ) const;

	// Проверка синтаксиса для определенных операций - например sin требует аргумент в { } скобках.
	bool checkUnaryBracketsSyntax( int currentPosition, const std::string& expectedBracketsId ) const;

	// Проверка синтаксиса для сложных операций, например sqrt[]{}.
	bool checkBinaryBracketsSyntax( int currentPosition );

	bool isOpenBracket( const std::string& token ) const { return (token == "(") || (token == "{") || (token == "["); }

	bool isCloseBracket( const std::string& token ) const { return (token == ")") || (token == "}") || (token == "]"); }
	
	// Обработка связанная с закрытием скобки, то есть концом некоторого выражения.
	void processClosingBracket();
	
	// Обработка бинарных операций/операторов.
	void processBinaryOperator();

	// Некоторых токены должны быть положены в стек в самом конце и никак не связанным в данном контексте с другими.
	// Выводим эти токены в дерево.
	void dumpToOutput();
};