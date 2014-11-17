// Автор: Воротилов Владислав.

// Описание: Класс определяющий тип токена.

#pragma once
#include <string>
#include <regex>

// Определяет всевозможные типы токенов в нашей задаче.
enum TTokenType {
	TT_Number,
	TT_Variable,
	TT_BinOp, 
	TT_UnaryOp,
	TT_SimpleSqrt,
	TT_ComplexSqrt,
	TT_Frac,
	TT_Cos,
	TT_Sin,
	TT_Tan,
	TT_Ctan,
	TT_Pow,
	TT_Sum,
	TT_Mul,
	TT_Assign,
	TT_OpenBracket,
	TT_CloseBracket,
	TT_LowIndex,
	TT_UnknownOperator,
	TT_BoolOp,
	TT_ComparisonOp,
	TT_BoolExp,

	// Тип нужен для проверки вычислимости, при парсинге такой тип никогда не создается
	TT_Computable,
	TT_ComputableBool
};

// Тип для токена после парсинга ввода.
class CTexToken {
public:

	CTexToken( TTokenType _tokenType, const std::string& _tokenString )
		: tokenType( _tokenType )
		, tokenString( _tokenString ) 
	{}

	TTokenType GetType() const { return tokenType; }

	const std::string& GetString() const { return tokenString; }

	// Определяет тип токена.
	static CTexToken ConvertToken( const std::string& token ); 

private:
	// Тип токена.
	TTokenType tokenType;
	// Содержимое строки, определяющей токен.
	std::string tokenString;
	
	// Регулярные выражения для определения типов токенов.
	static const std::tr1::regex numberRegex; 
	static const std::tr1::regex variableRegex;
};