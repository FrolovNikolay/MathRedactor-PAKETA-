// Автор: Воротилов Владислав.

#include <TokenTypes.h>
#include <cassert>

// Числа с плавающей точкой и десятичные.

const std::tr1::regex CTexToken::numberRegex( "([0-9]+\\.[0-9]*)|(\\.[0-9]+)|([0-9]+)" );
const std::tr1::regex CTexToken::variableRegex( "[a-z]*" );

CTexToken  CTexToken::ConvertToken( const std::string& token )
{
	bool tokenIsNumber = std::regex_match( token, numberRegex );
	bool tokenIsVariable = std::regex_match( token, variableRegex );
	if( tokenIsNumber ) {
		return CTexToken( TT_Number, token );
	} else if( tokenIsVariable ) {
		return CTexToken( TT_Variable, token );
	} else if( token == "+" || token == "-" || token == "*" || token == "/" ) {
		return CTexToken( TT_BinOp, token );
	} else if( token == "\\cos" ) {
		return CTexToken( TT_Cos, token );
	} else if( token == "\\sin" ) {
		return CTexToken( TT_Sin, token );
	} else if( token == "\\tg" ) {
		return CTexToken( TT_Tan, token );
	} else if( token == "\\ctg" ) {
		return CTexToken( TT_Ctan, token );
	} else if( token == "\\sum" ) {
		return CTexToken( TT_Sum, token );
	} else if( token == "\\sqrt" ) {
		return CTexToken( TT_SimpleSqrt, token );
	} else if( token == "\\sqrt[" ) {
		return CTexToken( TT_ComplexSqrt, token );
	} else if( token == "\\frac" ) {
		return CTexToken( TT_Frac, token );
	} else if( token == "\\mul" ) {
		return  CTexToken( TT_Mul, token );
	} else if( token == "=" ) {
		return CTexToken( TT_Assign, token );
 	} else if( token == "(" || token == "[" || token == "{" ) {
		return CTexToken( TT_OpenBracket, token );
	} else if( token == "^" ) {
		return CTexToken( TT_Pow, token );
	} else if( token == "_" ) {
		return CTexToken( TT_LowIndex, token );
	} else if( token == "-#" || token == "+#" ) {
		return CTexToken( TT_UnaryOp, token );
	} else if( token == ")" || token == "]" || token == "}" ) {
		return CTexToken( TT_CloseBracket, token );
	} else if( token == "<" || token == ">" || token == "\\le" || token == "\\ge" || token == "\\ne" ) {
		return CTexToken( TT_ComparisonOp, token );
	} else if( token == "\\vee" || token == "\\wedge" ) {
		return CTexToken( TT_BoolOp, token );
	} else {
		// Не поддерживаемые строки никогда сюда не попадут - они обработаются в NotationBuilder.
		assert( false );
	}
}
