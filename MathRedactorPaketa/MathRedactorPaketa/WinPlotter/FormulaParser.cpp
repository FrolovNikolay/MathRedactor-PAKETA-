// Автор: Федюнин Валерий

#include "FormulaParser.h"

#include <assert.h>
#include <vector>
#include <set>
#include <algorithm>

namespace {
	
	// возвращает размер пространства, в котором задается формула
	int GetSpaceDimension( const std::string& text ) 
	{
		int res = 3;
		if( text.find( 'x' ) == std::string::npos ) {
			res--;
		}
		if( text.find( 'y' ) == std::string::npos ) {
			res--;
		}
		if( text.find( 'z' ) == std::string::npos ) {
			res--;
		}
		return res;
	}

	bool IsParameterT( const std::string& text, int i ) {
		return( text[i] == 't' && ( i == 0 || text[i - 1] != 'r' ) && ( i + 1 < text.size() || text[i + 1] != 'g' ) );
	}

	bool IsParameterL( const std::string& text, int i ) {
		return( text[i] == 'l' && ( i == 0 || text[i - 1] != 'u' ) );
	}

	int GetPlotDimension( const std::string& text )
	{
		int nonParametricDimension = GetSpaceDimension( text ) - 1;
		bool tParameterFound = false;
		bool lParameterFound = false;
		for( int i = 0; i < text.size(); ++i ) { 
			if( IsParameterT( text, i ) ) {
				tParameterFound = true;
			}
			if( IsParameterL( text, i ) ) {
				lParameterFound = true;
			}
		}
		if( !tParameterFound && !lParameterFound ) {
			return nonParametricDimension;
		}
		return static_cast<int>( tParameterFound ) + static_cast<int>( tParameterFound );
	}

	void CheckEquationsCorrectness( const std::vector<std::string>& equations )
	{
		std::string variables = "xyz";
		for( int i = 0; i < equations.size(); ++i ) {
			assert( variables.find( equations[i][0] ) != std::string::npos );
			assert( equations[i][1] == '=' );
		}
	}

	std::vector<char> GetEquationsVariables( const std::vector<std::string>& equations )
	{
		std::vector<char> res;
		std::string variables = "xyz";
		for( int i = 0; i < equations.size(); ++i ) {
			for( int j = 2; j < equations[i].size(); ++j ) {
				if( IsParameterL( equations[i], j ) ) {
					res.push_back( 'l' );
				} else if( IsParameterT( equations[i], j ) ) {
					res.push_back( 't' );
				} else if( variables.find( equations[i][j] ) != std::string::npos ) {
					res.push_back( equations[i][j] );
				}
			}
		}
		std::sort( res.begin(), res.end() );
		res.resize( std::distance( res.begin(), std::unique( res.begin(), res.end() ) ) );
		return res;
	}

	bool ExtraBrackets( const std::string& equation, int left, int right ) 
	{
		if( equation[left] != '(' || equation[right - 1] != ')' ) {
			return false;
		}
		int balance = 0;
		for( int i = left + 1; i < right - 1; ++i ) {
			if( equation[i] == '(' ) {
				balance++;
			} else if( equation[i] == ')' ) {
				balance--;
				if( balance < 0 ) {
					return false;
				}
			}
		}

		return true;
	}

	IOperator* ParseOperator( const std::string& equation, int left, int right );

	IOperator* ParseBinaryOperator( const std::string& equation, int left, int right )
	{
		assert( left < right );
		std::string operators = "-+/*^";

		for( int i = 0; i < operators.size(); ++i ) {
			int balance = 0;
			for( int j = right - 1; j >= left; --j ) {
				if( equation[j] == '(' ) {
					++balance;
				} else if( equation[j] == ')' ) {
					--balance;
				} else if( equation[j] == operators[i] && balance == 0 && j > left && j < right - 1 ) {
					IOperator* leftOperand = ParseOperator( equation, left, j );
					IOperator* rightOperand = ParseOperator( equation, j + 1, right );
					BINOP type;
					switch( operators[i] ) {
					case '-':
						type = MINUS;
						break;
					case '+':
						type = PLUS;
						break;
					case '/':
						type = DIV;
						break;
					case '*':
						type = TIMES;
						break;
					case '^':
						type = POWER;
						break;
					}
					return new CBinaryOperator( leftOperand, rightOperand, type );
				}
			}
		}

		return 0;
	}

	IOperator* ParseFunction( const std::string& equation, int left, int right )
	{
		assert( left < right );

		std::string functionNames[] = {"sin", "cos", "tg", "ctg", "sqrt", "-"};
		FUNC types[] = {SIN, COS, TG, CTG, SQRT, UNARY_MINUS};

		for( int i = 0; i < 6; ++i ) {
			if( left + functionNames[i].size() < right 
				&& equation.substr( left, functionNames[i].size() ) == functionNames[i] ) 
			{
				return new CFunction( ParseOperator( equation, left + functionNames[i].size(), right ), types[i] );
			}
		}

		return 0;
	}

	IOperator* ParseVariable( const std::string& equation, int left, int right )
	{
		std::string variableNames = "xyzlt";

		if( left + 1 == right && variableNames.find( equation[left] ) != std::string::npos ) {
			return new CVariable( equation[left] );
		}

		return 0;
	}

	IOperator* ParseConstant( const std::string& equation, int left, int right )
	{
		try {
			double value = std::stod( equation.substr( left, right - left ) );
			return new CConstant( value );
		} catch( const std::invalid_argument& ex ) {
			return 0;
		}
	}

	IOperator* ParseOperator( const std::string& equation, int left, int right )
	{
		IOperator* res = 0;

		while( ExtraBrackets( equation, left, right ) ) {
			left++;
			right--;
		}

		res = ParseBinaryOperator( equation, left, right );
		if( res != 0 ) {
			return res;
		}

		res = ParseFunction( equation, left, right );
		if( res != 0 ) {
			return res;
		}

		res = ParseVariable( equation, left, right );
		if( res != 0 ) {
			return res;
		}

		res = ParseConstant( equation, left, right );
		if( res != 0 ) {
			return res;
		}
		
		assert( false );
	}

	CEquation ParseEquation( const std::string& equation ) 
	{
		IOperator* root = ParseOperator( equation, 2, equation.size() );
		char resultVariable = equation[0];
		return CEquation( resultVariable, root );
	}
};

CFormula ParseFormula( const std::string& text ) {
	int spaceDimension = GetSpaceDimension( text );
	assert( spaceDimension >= 2 );
	int plotDimension = GetPlotDimension( text );
	std::vector<std::string> equations( 1 );
	std::string spaces = " \t\n\r";
	for( int i = 0; i < text.size(); ++i ) {
		if( spaces.find( text[i] ) != std::string::npos ) {
			continue;
		}
		if( text[i] == ',' && !equations.back().empty() ) {
			equations.push_back( std::string() );
			continue;
		}
		equations.back().push_back( text[i] );
	}
	CheckEquationsCorrectness( equations );
	std::vector<char> variables = GetEquationsVariables( equations );
	CFormula formula( spaceDimension, plotDimension, variables );
	for( int i = 0; i < equations.size(); ++i ) {
		formula.AddEquation( ParseEquation( equations[i] ) );
	}
	return formula;
}