// Автор: Федюнин Валерий

#include "FormulaParser.h"

#include <assert.h>
#include <vector>
#include <set>
#include <algorithm>

namespace {
	
	// возвращает размер пространства, в котором задается формула
	int GetSpaceDimension( const std::vector<std::string>& equations ) 
	{
		if( equations.size() == 2 || ( equations.size() == 1 && equations[0][0] == 'y' ) ) {
			return 2;
		}
		return 3;
	}

	// возвращает индекс парной скобки
	int GetMatchingBracket( const std::string& text, int i ) 
	{
		assert( text[i] == '(' );
		assert( i != text.length() - 1 );
		int bracketBalance = 1;
		i++;
		do {
			if( text[i] == '(' ) {
				bracketBalance++;
			} else if( text[i] == ')' ) {
				bracketBalance--;
				if( bracketBalance == 0 ) {
					return i;
				}
			} 
			i++;
		} while( i < text.length() );
		assert( i != text.length() );
	}

	// является ли данный символ переменной t
	bool IsParameterT( const std::string& text, int i ) 
	{
		return( text[i] == 't' && ( i == 0 || text[i - 1] != 'r' ) 
			&& ( i + 1 >= static_cast<int>( text.size() ) || text[i + 1] != 'g' ) );
	}

	// является ли данный символ переменной l
	bool IsParameterL( const std::string& text, int i ) 
	{
		return( text[i] == 'l' && ( i == 0 || text[i - 1] != 'u' ) );
	}

	// получить размерность графика (т.е. кол-во его параметров: 2 - поверхность. 1 - кривая)
	int GetPlotDimension( const std::vector<std::string>& equations )
	{
		if( equations.size() != 1 ) {
			bool lFound = false, tFound = false;
			for( int i = 0; i < static_cast<int>( equations.size() ); ++i ) {
				for( int j = 0; j < static_cast< int >( equations[i].length() ); ++j ) {
					if( IsParameterT( equations[i], j ) ) {
						tFound = true;
					} else if( IsParameterL( equations[i], j ) ) {
						lFound = true;
					}
				}
			}
			return std::max( 1, static_cast< int >( lFound ) +static_cast< int >( tFound ) );
		} else {
			if( equations[0][0] == 'y' ) {
				return 1;
			} else {
				return 2;
			}
		}
	}

	// проверить префиксы уравнений
	void CheckEquationsCorrectness( const std::vector<std::string>& equations )
	{
		std::string variables = "xyz";
		for( int i = 0; i < static_cast<int>( equations.size() ); ++i ) {
			assert( variables.find( equations[i][0] ) != std::string::npos );
			assert( equations[i][1] == '=' );
		}
	}

	// получить переменные, исопльзуемые в уравнении
	std::vector<char> GetEquationsVariables( const std::vector<std::string>& equations )
	{
		std::vector<char> res;
		
		if( equations.size() == 1 ) {
			res.push_back( 'x' );
			if( equations[0][0] == 'z' ) {
				res.push_back( 'y' );
			}
		} else {
			bool tFound = false, lFound = false;
			for( int i = 0; i < static_cast<int>( equations.size() ); ++i ) {
				for( int j = 0; j < static_cast< int >( equations[i].length() ); ++j ) {
					if( IsParameterT( equations[i], j ) ) {
						tFound = true;
					} else if( IsParameterL( equations[i], j ) ) {
						lFound = true;
					}
				}
			}
			if( lFound ) {
				res.push_back( 'l' );
			}
			if( tFound || res.empty() ) {
				res.push_back( 't' );
			}
		}
		return res;
	}

	// являются ли 2 символа парными скобкаи 
	bool PairBrackets( const std::string& equation, int left, int right ) 
	{
		if( equation[left] != '(' || equation[right] != ')' ) {
			return false;
		}
		return( GetMatchingBracket( equation, left ) == right );
	}

	// основная функция, парсит один оператор в данной подстроке (им может быть и вызов функции и число)
	// объявление вынесено для возможностей рекурсивного вызова
	IOperator* ParseOperator( const std::string& equation, int left, int right );

	// парсит бинарный оператор, возвращает 0 при неудаче
	IOperator* ParseBinaryOperator( const std::string& equation, int left, int right )
	{
		assert( left < right );
		char operators[3][2] = {{'-', '+'}, {'/', '*'}, {'^', '^'}};

		for( int i = 0; i < 3; ++i ) {
			int balance = 0;
			for( int j = right - 1; j >= left; --j ) {
				if( equation[j] == '(' ) {
					++balance;
				} else if( equation[j] == ')' ) {
					--balance;
				} else if( ( equation[j] == operators[i][0] || equation[j] == operators[i][1] ) 
					&& balance == 0 && j > left && j < right - 1 ) 
				{
					IOperator* leftOperand = ParseOperator( equation, left, j );
					IOperator* rightOperand = ParseOperator( equation, j + 1, right );
					BINOP type;
					switch( equation[j] ) {
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

	// парсит вызов функции, возвращает 0 при неудаче
	IOperator* ParseFunction( const std::string& equation, int left, int right )
	{
		assert( left < right );

		std::string functionNames[] = {"sin", "cos", "tg", "ctg", "sqrt", "-"};
		FUNC types[] = {SIN, COS, TG, CTG, SQRT, UNARY_MINUS};

		for( int i = 0; i < 6; ++i ) {
			if( left + static_cast<int>( functionNames[i].size() ) < right
				&& equation.substr( left, functionNames[i].size() ) == functionNames[i] ) 
			{
				return new CFunction( ParseOperator( equation, left + functionNames[i].size(), right ), types[i] );
			}
		}

		return 0;
	}

	// парсит обращение к переменной, возвращает 0 при неудаче
	IOperator* ParseVariable( const std::string& equation, int left, int right )
	{
		if( left + 1 == right && equation[left] >= 'a' && equation[left] <= 'z' ) {
			return new CVariable( equation[left] );
		}

		return 0;
	}

	// парсит константу (число), возвращает 0 при неудаче
	IOperator* ParseConstant( const std::string& equation, int left, int right )
	{
		try {
			double value = std::stod( equation.substr( left, right - left ) );
			return new CConstant( value );
		} catch( const std::invalid_argument& ) {
			return 0;
		}
	}

	// парсит множественную операцию
	IOperator* ParseSetOperation( const std::string& equation, int left, int right )
	{
		// минимальная длина выражения вида sum(i=0;2;x)
		if( right - left < 12 ) {
			return 0;
		}
		SETOPTYPE type;
		if( equation.substr( left, 3 ) == "mul" ) {
			type = MUL;
		} else if( equation.substr( left, 3 ) == "sum" ) {
			type = SUM;
		} else {
			return 0;
		}
		if( equation[left + 3] != '(' || equation[right - 1] != ')' || equation[left + 5] != '=' ) {
			return 0;
		}
		char variable = equation[left + 4];
		if( variable < 'a' || variable > 'z' ) {
			return 0;
		}
		std::vector<int> semicolons;
		for( int i = left; i < right && semicolons.size() < 2; ++i ) {
			if( equation[i] == ';' ) {
				semicolons.push_back( i );
			}
		}
		if( semicolons.size() < 2 ) {
			return 0;
		}
		IOperator* begin = ParseOperator( equation, left + 6, semicolons[0] );
		if( begin == 0 ) {
			return 0;
		}
		IOperator* condition = ParseOperator( equation, semicolons[0] + 1, semicolons[1] );
		if( condition == 0 ) {
			delete( begin );
			return 0;
		}
		IOperator* expression = ParseOperator( equation, semicolons[1] + 1, right - 1 );
		if( expression == 0 ) {
			delete( begin );
			delete( condition );
			return 0;
		}
		return new CSetOperator( variable, expression, begin, condition, type );
	}

	IOperator* ParseOperator( const std::string& equation, int left, int right )
	{
		IOperator* res = 0;

		while( PairBrackets( equation, left, right - 1 ) ) {
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

		res = ParseSetOperation( equation, left, right );
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

	// Приводит одну множественную операцию к корректному виду
	// Из sum( pre , post ) func в sum( pre , post, func ) 
	std::string PreParseSetOperator( const std::string& _equation, int index );

	// найти конец токена (выхов функции, выражения в скобках, обращения к переменной, константы )
	int GetTokenEnd( std::string& text, int index ) {
		int end;
		if( text[index] == '(' ) {
			// третий аргумент нечто в скобках
			end = GetMatchingBracket( text, index );
		} else if( text[index] >= 'a'  &&  text[index] <= 'z'  
			&&  ( index + 1 == text.length() || text[index + 1] < 'a' || text[index + 1] > 'z') )
		{
			// третий аргумент - переменная
			end = index;
		} else if( text[index] >= 'a'  &&  text[index] <= 'z' ) {
			// третий аргумент - вызов функции
			end = index;
			while( end + 1 < text.length() &&
				text[end + 1] >= 'a' && text[end + 1] <= 'z' )
			{
				end++;
			}
			std::string functionName = text.substr( index, end - index + 1 );
			if( functionName == "mul" || functionName == "sum" ) {
				text = PreParseSetOperator( text, index );
			}
			end = GetMatchingBracket( text, end + 1 );
		} else if( text[index] >= '0'  &&  text[index] <= '9' ) {
			// третий аргумент - константа
			end = index;
			while( end + 1 < text.length() &&
				text[end + 1] >= '0' && text[end + 1] <= '9' )
			{
				end++;
			}
		}
		return end;
	}

	std::string PreParseSetOperator( const std::string& _equation, int index ) {
		std::string equation( _equation );
		// игнорируем название
		int leftBracket = index + 3;
		int rightBracket = GetMatchingBracket( equation, leftBracket );
		int argsNum = 1;
		for( int i = leftBracket; i <= rightBracket; ++i ) {
			if( equation[i] == ';' ) {
				++argsNum;
			}
		}
		// уже в требуемом виде
		if( argsNum == 3 ) {
			return _equation;
		}

		assert( rightBracket < equation.length() - 1 );
		assert( argsNum > 1  &&  argsNum < 4 );
		
		index = rightBracket + 1;
		// конец и начало аргумента. который надо сделать третьим у функции (правый конец включается)
		int argumentBegin = index; 
		int argumentEnd = GetTokenEnd( equation, index );
		
		// если была задана степень
		if( argumentEnd + 1 < equation.length() && equation[argumentEnd + 1] == '^' ) {
			argumentEnd = GetTokenEnd( equation, argumentEnd + 2 );
		}

		return _equation.substr( 0, rightBracket ).append( ";" )
			.append( _equation.substr( argumentBegin, argumentEnd - argumentBegin + 1 ) )
			.append( ")" ).append( _equation.substr( argumentEnd + 1 ) );
	}

	// Приводит операторы mul, sum к виду, с которым умеет работать Plotter
	std::string PreParseEquation( const std::string& _equation ) {
		std::string equation( _equation );
		int i = std::min( equation.find( "mul" ), equation.find( "sum" ) );
		while( i != std::string::npos ) {
			equation = PreParseSetOperator( equation, i );
			i = std::min( equation.find( "mul", i + 1 ), equation.find( "sum", i + 1 ) );
		}
		return equation;
	}

	// Парсит одно уравнение
	CEquation ParseEquation( const std::string& _equation ) 
	{
		std::string equation = PreParseEquation( _equation );
		IOperator* root = ParseOperator( equation, 2, equation.size() );
		char resultVariable = equation[0];
		return CEquation( resultVariable, root );
	}
};

// Парсит всю формулу (которая может содержать несоклько уравнений)
CFormula ParseFormula( const std::string& text ) {
	std::vector<std::string> equations( 1 );
	std::string spaces = " \t\n\r";
	for( int i = 0; i < static_cast<int>( text.size() ); ++i ) {
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
	int spaceDimension = GetSpaceDimension( equations );
	assert( spaceDimension >= 2 );
	int plotDimension = GetPlotDimension( equations );
	std::vector<char> variables = GetEquationsVariables( equations );
	CFormula formula( spaceDimension, plotDimension, variables );
	for( int i = 0; i < static_cast<int>( equations.size() ); ++i ) {
		formula.AddEquation( ParseEquation( equations[i] ) );
	}
	return formula;
}