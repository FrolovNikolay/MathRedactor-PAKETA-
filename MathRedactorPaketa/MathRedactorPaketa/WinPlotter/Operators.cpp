// Автор: Федюнин Валерий

#include "Operators.h"

#include <assert.h>

// CConst

CConstant::CConstant( double value ) : value( value )
{
}

double CConstant::Calculate( const std::map<char, double>& variables ) const
{
	return value;
}

// CVariable

CVariable::CVariable( char name )
{
	variableName = name;
}

double CVariable::Calculate( const std::map<char, double>& variables ) const
{
	assert( variables.find( variableName ) != variables.end() );
	return variables.find( variableName )->second;
}

// CBinaryOperator

CBinaryOperator::CBinaryOperator( IOperator* left, IOperator* right, BINOP type ) 
	: left( left ), right( right ), type( type )
{
	assert( left != 0 );
	assert( right != 0 );
}

double CBinaryOperator::Calculate( const std::map<char, double>& variables ) const
{
	double leftValue = left->Calculate( variables );
	double rightValue = right->Calculate( variables );

	switch( type ) {
	case PLUS:
		return leftValue + rightValue;
	case MINUS:
		return leftValue - rightValue;
	case TIMES:
		return leftValue * rightValue;
	case DIV:
		return leftValue / rightValue;
	case POWER:
		return std::pow( leftValue, rightValue );
	default:
		assert( false );
	}
}

// CFunction

CFunction::CFunction( IOperator* parameter, FUNC type ) : parameter( parameter ), type( type )
{
	assert( parameter != 0 );
}

double CFunction::Calculate( const std::map<char, double>& variables ) const
{
	double parameterValue = parameter->Calculate( variables );

	switch( type ) {
	case SIN:
		return std::sin( parameterValue );
	case COS:
		return std::cos( parameterValue );
	case SQRT:
		return std::sqrt( parameterValue );
	case TG:
		return std::tan( parameterValue );
	case CTG:
		return 1 / std::tan( parameterValue );
	case UNARY_MINUS:
		return -parameterValue;
	default:
		assert( false );
	}
}