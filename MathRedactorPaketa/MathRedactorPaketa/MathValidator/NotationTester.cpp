// Автор: Николай Фролов

#include <NotationTester.h>
#include <ErrorCatcher.h>
#include <Utils.h>
#include <cassert>

// Метод тестирования валидности формулы в виде latex-строки.
bool CNotationTester::Test( const std::string& src, const std::set<std::string>& _knownVars )
{
	reinitialize();

	CNotationBuilder builder( src );
	std::stack< std::pair<std::string, int> > notation = builder.GetReverseNotation();
	knownVars = _knownVars;
	tokens = builder.GetTokens();

	while( !notation.empty() ) {
		currentNotation = notation.top();
		notation.pop();
		CTexToken currToken = CTexToken::ConvertToken( currentNotation.first );
		switch( currToken.GetType() ) {
			case TT_Number:
				numbersStack.push( currToken );
				break;
			case TT_Variable:
				numbersStack.push( currToken );
				break;
			case TT_BoolOp:
			case TT_ComparisonOp:
			case TT_BinOp:
			case TT_Frac:
			case TT_ComplexSqrt:
				testBinaryOperation( currToken );
				break;
			case TT_Pow:
				// Особенность написания аггрегирующих операторов sum/mul
				if( !notation.empty() && IsAggregationFunction( notation.top().first ) ) {
					break;
				}
				testBinaryOperation( currToken );
				break;
			case TT_UnaryOp:
			case TT_Cos:
			case TT_Sin:
			case TT_Tan:
			case TT_Ctan:
			case TT_SimpleSqrt:
				testUnaryOperation( currToken );
				break;
			case TT_Assign:
				// Допускается определение переменных только в функциях аггрегаторах.
				// Если не выполняется следующее условие, значит мы не определяем переменную в аггрегаторе и это ошибка.
				if( notation.empty() || notation.top().first != "_" ) {
					// Ошибка - некорретктное использование =
					throw CErrorCatcher( "Assign error", tokens, notation.top().second );
				}
				testAssignOperation();
				break;
			case TT_Sum:
			case TT_Mul:
				testAggregationOperation( currToken );
				break;
			case TT_LowIndex:
				// Индексация была пока никак не обрабатывается.
				break;
			default:
				assert( false );
		}
	}
	if( numbersStack.size() == 1 && numbersStack.top().GetType() == TT_Variable && knownVars.find( numbersStack.top().GetString() ) == knownVars.end() ) {
		throw CErrorCatcher( "Unknown variable " + numbersStack.top().GetString(), tokens, currentNotation.second );
	} else if( numbersStack.size() != 1 || ( numbersStack.top().GetType() != TT_Computable && numbersStack.top().GetType() != TT_Number ) ) {
		// Ошибка, итоговой результат несвязен, либо не является вычислимым. - Здесь ошибочно все выражение, точнее выделить ошибку сложно.
		throw CErrorCatcher( "Error", tokens, -1 );
	}
	return true;
}

// Проверка унарной операции(например sin) на корректность, в соответствии с деревом.
void CNotationTester::testUnaryOperation( const CTexToken& operation )
 {
	if( numbersStack.empty() ) {
		// Ошибка - недостаточно аргументов для операции.
		throw CErrorCatcher( "Not enough arguments", tokens, currentNotation.second );
	}
	CTexToken arg( numbersStack.top() );
	numbersStack.pop();
	numbersStack.push( tokenUnaryOperation( operation, arg ) );
}

// Построение нового токена по унарной операции и аргументу в зависимости от их типов.
CTexToken CNotationTester::tokenUnaryOperation( const CTexToken& operation, const CTexToken& arg ) const
{
	if( arg.GetType() == TT_Variable && knownVars.find( arg.GetString() ) == knownVars.end() ) {
		throw CErrorCatcher( "Unknown variable " + arg.GetString(), tokens, currentNotation.second );
	}
	if( arg.GetType() == TT_Number ) {
		switch( operation.GetType() ) {
			case TT_UnaryOp:
				if( operation.GetString() == "-#" ) {
					return CTexToken( TT_Number, std::to_string( -std::stod( arg.GetString() ) ) );
				} else if( operation.GetString() == "+#" ) {
					return CTexToken( TT_Number, arg.GetString() );
				} else {
					assert( false );
				}
			case TT_Sin:
				return CTexToken( TT_Number, std::to_string( std::sin( std::stod( arg.GetString() ) ) ) );
			case TT_Cos:
				return CTexToken( TT_Number, std::to_string( std::cos( std::stod( arg.GetString() ) ) ) );
			case TT_Tan:
				return CTexToken( TT_Number, std::to_string( std::tan( std::stod( arg.GetString() ) ) ) );
			case TT_Ctan:
				return CTexToken( TT_Number, std::to_string( std::tan( std::stod( arg.GetString() ) ) ) );
			case TT_SimpleSqrt:
				return CTexToken( TT_Number, std::to_string( sqrt( std::stod( arg.GetString() ) ) ) );
			default:
				assert( false );
		}
	} else if( arg.GetType() == TT_Computable || arg.GetType() == TT_Variable ) {
		switch( operation.GetType() ) {
			case TT_UnaryOp:
				return CTexToken( TT_Computable, operation.GetString() + arg.GetString() );

			case TT_SimpleSqrt:
			case TT_Sin:
			case TT_Cos:
				return CTexToken( TT_Computable, operation.GetString() + "{" + arg.GetString() + "}" );

			case TT_Tan:
			case TT_Ctan:
				return CTexToken( TT_Computable, operation.GetString() + "(" + arg.GetString() + ")" );

			default:
				assert( false );
		}
	} else {
		throw CErrorCatcher( "Incorrect arguemnt's types", tokens, currentNotation.second );
	}
}

// Проверка бинарной операции(например * или frac{}{}) на корректность, в соответствии с деревом.
void CNotationTester::testBinaryOperation( const CTexToken& operation )
{
	if( numbersStack.size() < 2 ) {
		// Ошибка - недостаточно аргументов для операции.
		throw CErrorCatcher( "Not enough arguments", tokens, currentNotation.second );
	}
	CTexToken arg2 = numbersStack.top();
	numbersStack.pop();
	CTexToken arg1 = numbersStack.top();
	numbersStack.pop();
	numbersStack.push( tokenBinaryOperation( operation, arg1, arg2 ) );
}

// Построение нового токена по бинарной операции и ее аргументам в зависимости от их типов.
CTexToken CNotationTester::tokenBinaryOperation( const CTexToken& operation, const CTexToken& arg1, const CTexToken& arg2 ) const
{
	if( arg1.GetType() == TT_Variable && knownVars.find( arg1.GetString() ) == knownVars.end() ) {
		throw CErrorCatcher( "Unknown variable " + arg1.GetString(), tokens, currentNotation.second );
	}
	if( arg2.GetType() == TT_Variable && knownVars.find( arg2.GetString()) == knownVars.end() ) {
		throw CErrorCatcher( "Unknown variable " + arg2.GetString(), tokens, currentNotation.second );
	}

	if( arg1.GetType() == TT_Number && arg2.GetType() == TT_Number ) {
		switch( operation.GetType() ) {
			case TT_BinOp:
				if( operation.GetString() == "+" ) {
					return CTexToken( TT_Number, std::to_string( std::stod( arg1.GetString() ) + std::stod( arg2.GetString() ) ) );
				} else if( operation.GetString() == "-" ) {
					return CTexToken( TT_Number, std::to_string( std::stod( arg1.GetString() ) - std::stod( arg2.GetString() ) ) );
				} else if( operation.GetString() == "/" ) {
					if( std::stod( arg2.GetString() ) == 0 ) {
						throw CErrorCatcher( "Division by zero", tokens, currentNotation.second );
					}
					return CTexToken( TT_Number, std::to_string( std::stod( arg1.GetString() ) / std::stod( arg2.GetString() ) ) );
				} else if( operation.GetString() == "*" ) {
					return CTexToken( TT_Number, std::to_string( std::stod( arg1.GetString() ) * std::stod( arg2.GetString() ) ) );
				}
			case TT_Pow:
				if( std::stod( arg1.GetString() ) == 0 && std::stod( arg2.GetString() ) <= 0 ) {
					throw CErrorCatcher( "Power of zero in non-postive degree", tokens, currentNotation.second );
				}
				return CTexToken( TT_Number, std::to_string( pow( std::stod( arg1.GetString() ), std::stod( arg2.GetString() ) ) ) );
			case TT_ComplexSqrt:
				if( std::stod( arg1.GetString() ) == 0 ) {
					throw CErrorCatcher( "Zero degree square", tokens, currentNotation.second );
				}
				return CTexToken( TT_Number, std::to_string( pow( std::stod( arg2.GetString() ), 1.0 / std::stod( arg1.GetString() ) ) ) );
			case TT_Frac:
				if( std::stod( arg2.GetString() ) == 0 ) {
					throw CErrorCatcher( "Division by zero", tokens, currentNotation.second );
				}
				return CTexToken( TT_Number, std::to_string( std::stod( arg1.GetString() ) / std::stod( arg2.GetString() ) ) );
			case TT_ComparisonOp:
				if( operation.GetString() == "<" ) {
					return CTexToken( TT_BoolExp, std::to_string( std::stod( arg1.GetString() ) < std::stod( arg2.GetString() ) ) );
				} else if( operation.GetString() == ">" ) {
					return CTexToken( TT_BoolExp, std::to_string( std::stod( arg1.GetString() ) > std::stod( arg2.GetString() ) ) );
				} else if( operation.GetString() == "\\ne" ) {
					return CTexToken( TT_BoolExp, std::to_string( std::stod( arg1.GetString() ) != std::stod( arg2.GetString() ) ) );
				} else if( operation.GetString() == "\\le" ) {
					return CTexToken( TT_BoolExp, std::to_string( std::stod( arg1.GetString() ) <= std::stod( arg2.GetString() ) ) );
				} else if( operation.GetString() == "\\ge" ) {
					return CTexToken( TT_BoolExp, std::to_string( std::stod( arg1.GetString() ) >= std::stod( arg2.GetString() ) ) );
				}
			default:
				assert( false );
		}
	} else if( ( arg1.GetType() == TT_BoolExp || arg1.GetType() == TT_ComputableBool ) 
			   && ( arg2.GetType() == TT_BoolExp || arg2.GetType() == TT_ComputableBool ) && operation.GetType() != TT_BinOp ) 
	{
		if( arg1.GetType() == TT_ComputableBool || arg2.GetType() == TT_ComputableBool ) {
			return CTexToken( TT_ComputableBool, arg1.GetString() + operation.GetString() + arg2.GetString() );
		} else {
			if( operation.GetString() == "\\vee" ) {
				return CTexToken( TT_BoolExp, std::to_string( std::stod( arg1.GetString() ) || std::stod( arg2.GetString() ) ) );
			} else if( operation.GetString() == "\\wedge" ) {
				return CTexToken( TT_BoolExp, std::to_string( std::stod( arg1.GetString() ) && std::stod( arg2.GetString() ) ) );
			} else {
				assert( false );
			}
		}
	} else if( ( arg1.GetType() == TT_Variable || arg1.GetType() == TT_Computable || arg1.GetType() == TT_Number )
			&& ( arg2.GetType() == TT_Variable || arg2.GetType() == TT_Computable || arg2.GetType() == TT_Number ) ) 
	{
		switch( operation.GetType() ) {
			case TT_Pow:
			case TT_BinOp:
				if( operation.GetString() == "/" && arg2.GetType() == TT_Number && std::stod( arg2.GetString() ) == 0 ) {
					throw CErrorCatcher( "Division by zero", tokens, currentNotation.second );
				}
				return CTexToken( TT_Computable, arg1.GetString() + operation.GetString() + arg2.GetString() );
			case TT_ComparisonOp:
				return CTexToken( TT_ComputableBool, arg1.GetString() + operation.GetString() + arg2.GetString() );
			case TT_ComplexSqrt:
				return CTexToken( TT_Computable, operation.GetString() + "[" + arg1.GetString() + "]" + "{" + arg2.GetString() + "}" );
			case TT_Frac:
				return CTexToken( TT_Computable, operation.GetString() + "{" + arg1.GetString() + "}" + "{" + arg2.GetString() + "}" );
			default:
				assert( false );
		}
	} else {
		throw CErrorCatcher( "Incorrect arguemnts' types", tokens, currentNotation.second );
	}
}

// Проверка аггрегирующих операций(например sum) на корректность, в соответствии с деревом.
void CNotationTester::testAggregationOperation( const CTexToken& operation )
{
	if( numbersStack.size() < 3 ) {
		// Ошибка - недостаточно аргументов для операции.
		throw CErrorCatcher( "Not enough arguments", tokens, currentNotation.second );
	}
	CTexToken aggregationStm = numbersStack.top();
	numbersStack.pop();
	CTexToken highArg = numbersStack.top();
	numbersStack.pop();
	CTexToken lowArg = numbersStack.top();
	numbersStack.pop();

	if( highArg.GetType() != TT_Number && highArg.GetType() != TT_Computable && highArg.GetType() != TT_ComputableBool
			&& ( highArg.GetType() == TT_Variable && knownVars.find( highArg.GetString() ) == knownVars.end() ) ) {
		// Ошибка - неподходящий тип верхней части аггрегатора.
		throw CErrorCatcher( "Incorrect high part of aggregation function", tokens, currentNotation.second );
	}
	if( highArg.GetType() == TT_BoolExp ) {
		throw CErrorCatcher( "Constant boolean is high part of aggregation function", tokens, currentNotation.second );
	}
	if( lowArg.GetType() != TT_Variable ) {
		// Ошибка - неподходящий тип нижней части аггрегатора.
		throw CErrorCatcher( "Incorrect low part of aggregation function", tokens, currentNotation.second );
	}
	if( aggregationStm.GetType() == TT_Number || aggregationStm.GetType() == TT_Computable 
			|| ( aggregationStm.GetType() == TT_Variable && knownVars.find( aggregationStm.GetString() ) != knownVars.end() ) ) {
		numbersStack.push( CTexToken( TT_Computable, operation.GetString() + "_" + lowArg.GetString() + "^" + highArg.GetString() + " " + aggregationStm.GetString() ) );
		knownVars.erase( sumVars.top() );
		sumVars.pop();
	} else {
		throw CErrorCatcher( "Incorrect statement in aggregation function", tokens, currentNotation.second );
	}
}

// Проверка присваивания на корректность.
void CNotationTester::testAssignOperation() 
{
	if( numbersStack.size() < 2 ) {
		throw CErrorCatcher( "Not enough arguments", tokens, currentNotation.second );
	}

	CTexToken second = numbersStack.top();
	numbersStack.pop();

	CTexToken first = numbersStack.top();
	numbersStack.pop();

	if( first.GetType() != TT_Variable ) {
		throw CErrorCatcher( "Assign no to variable isn't permit", tokens, currentNotation.second );
	} else {
		if( knownVars.find( first.GetString() ) != knownVars.end() ) {
			throw CErrorCatcher( "Variable redefinition " + first.GetString(), tokens, currentNotation.second );
		}
		if( second.GetType() == TT_Computable || second.GetType() == TT_Number
				|| ( second.GetType() == TT_Variable && knownVars.find( second.GetString() ) != knownVars.end() ) ) {
			numbersStack.push( CTexToken( TT_Variable, first.GetString() ) );
			knownVars.insert( first.GetString() );
			sumVars.push( first.GetString() );
		} else {
			throw CErrorCatcher( "Incorrect type in right side of assign", tokens, currentNotation.second );
		}
	}
}

// Обнуление внутренних значений для повторного использования.
void CNotationTester::reinitialize()
{
	while( !numbersStack.empty() ) {
		numbersStack.pop();
	}
	while( !sumVars.empty() ) {
		sumVars.pop();
	}
	knownVars.clear();
}