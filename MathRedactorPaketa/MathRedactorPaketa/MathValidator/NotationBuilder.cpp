// Автор : Николай Фролов.

#include <NotationBuilder.h>
#include <ErrorCatcher.h>
#include <assert.h>
#include <Utils.h>

// Все допустимые виды скобок.
const std::string CNotationBuilder::bracket[] = { "(", "{", "[", ")", "}", "]" };

// Размер массива скобок.
const int CNotationBuilder::bracArraySize = 6;

// Приоритеты всех видов операций.
const std::map< std::string, int > CNotationBuilder::operatorsPriority = {
	std::make_pair( "=", 0 ),
	std::make_pair( "+", 1 ),
	std::make_pair( "-", 2 ),
	std::make_pair( "*", 3 ),
	std::make_pair( "/", 3 ),
	std::make_pair( "^", 4 ),
	std::make_pair( "_", 6 ),
	std::make_pair( "-#", 5 ),
	std::make_pair( "+#", 5 ),
	std::make_pair( "\\tg", 5 ),
	std::make_pair( "\\ctg", 5 ),
	std::make_pair( "\\cos", 5 ),
	std::make_pair( "\\sin", 5 ),
	std::make_pair( "\\sqrt", 5 ),
	std::make_pair( "\\sqrt[", 5 ),
	std::make_pair( "\\frac", 5 ),
	std::make_pair( "\\sum", 3 ),
	std::make_pair( "\\mul", 3 ),

	std::make_pair( "<", 0 ),
	std::make_pair( ">", 0 ),
	std::make_pair( "\\le", 0 ),	// <=
	std::make_pair( "\\ge", 0 ),	// >=
	std::make_pair( "\\ne", 0 ),	// не равно
									// так как синтаксис латеховский, то не вводим отдельный символ равенства, а используем присваивание
	std::make_pair( "\\vee", -1 ),	// дизъюнкция
	std::make_pair( "\\wedge", -1 ),	// конъюнкция
};


// Регулярные выражения для различных видов токенов.
const std::regex CNotationBuilder::numberRegex( "([0-9]+\\.[0-9]*)|([0-9]+)|(\\.[0-9]+)" );
const std::regex CNotationBuilder::variableRegex( "[a-z]*" );
const std::regex CNotationBuilder::rightOpRegex( "([\\^_])" );
const std::regex CNotationBuilder::leftOpRegex( "[\\+\\-\\*/=]|([<>])|(\\\\((le)|(ge)|(ne)|(vee)|(wedge)))" );
const std::regex CNotationBuilder::unaryFunctionRegex( "\\\\((sin)|(cos)|(tg)|(ctg))" );
const std::regex CNotationBuilder::unarySpecifiedFunctionRegex( "\\\\((sqrt))" );
const std::regex CNotationBuilder::binaryFunctionRegex( "\\\\((sqrt)|(frac)|(sqrt\\[))" );
const std::regex CNotationBuilder::agregationFunctionRegex( "\\\\((mul)|(sum))" );

// Постороение дерева разбора на основе последовательности токенов.
// Принимает на вход формулу в формате latex.
// В случае некотроых синтаксических/лексических ошибок 
// генерируется исключение с описанием ошибки.
CNotationBuilder::CNotationBuilder( const std::string& src ) : tokens( ConsturctTokens( src ) ), mayUnary( true )
{
	// Проверка на корректную расстановку скобок.
	checkBrackets();

	// Обрабатываем по отдельности каждый токен и постепенно строим дерево разбора.
	for( int i = 0; i < static_cast<int>( tokens.size() ); ++i ) {	
		currentToken = tokens[i];
		// Функции одного аргумента, требующие аргумент в { } скобках.
		if( ( std::regex_match( currentToken, unarySpecifiedFunctionRegex ) && checkUnaryBracketsSyntax( i, "{" ) ) || std::regex_match( currentToken, unaryFunctionRegex ) ) {
			tempStack.push( std::pair<std::string, int>( currentToken, i ) );
			mayUnary = false;
		// Бинарные операторы с особым синтаксисом.
		} else if( std::regex_match( currentToken, binaryFunctionRegex ) && checkBinaryBracketsSyntax( i ) ) {
			processBinaryOperator();
			tempStack.push( std::pair<std::string, int>( currentToken, i ) );
		// Агрегирующие функции //sum, //mul.
		} else if( std::regex_match( currentToken, agregationFunctionRegex ) ) {
			tempStack.push( std::pair<std::string, int>( currentToken, i ) );
			mayUnary = false;
		// Обработка открывающейся скобки.
		} else if( isOpenBracket( currentToken ) ) {
			tempStack.push( std::pair<std::string, int>( "(", i ) );
			mayUnary = true;
		// Обработка выражения в скобках при их закрытии.
		} else if( isCloseBracket( currentToken ) ) {
			processClosingBracket();
		// Обработка обычных бинарных операторов.
		} else if( std::regex_match( currentToken, leftOpRegex ) || std::regex_match( currentToken, rightOpRegex ) ) {
			processBinaryOperator();
			tempStack.push( std::pair<std::string, int>( currentToken, i ) );
		// Помещение в дерево разбора чисел/переменных.
		} else if( std::regex_match( currentToken, numberRegex ) || std::regex_match( currentToken, variableRegex ) ) {
			outputNotation.push( std::pair<std::string, int>( currentToken, i ) );
			mayUnary = false;
		} else {
			// Ошибка - неизвестный тип токена
			throw CErrorCatcher( "Unknown token", tokens, i );
		}
	}

	// закидываем в дерево "не съеденные" другими операциями токены
	dumpToOutput();
}

// Проверяет распределение скобок в выражении на корректность.
void CNotationBuilder::checkBrackets() const
{
	for( int i = 0; i < static_cast<int>( tokens.size() ); ++i ) {
		for( int j = 0; j < bracArraySize; ++j ) {
			if( bracket[j] == tokens[i] ) {
				if( j < bracArraySize / 2 ) {
					int tempI = findClosingBracket( i );
					if( tempI == -1 ) {
						// выдать ошибку - не закрыта скобка
						throw CErrorCatcher( "Haven't closing bracket", tokens, i );
					} else if( i + 1 == tempI ) {
						// выдать ошибку - пара пустых скобок
						throw CErrorCatcher( "Empty brackets", tokens, i );
					} else {
						i = tempI;
					}
				} else {
					// выдать ошибку - закрывающая скобка без пары
					throw CErrorCatcher( "Haven't opening bracket", tokens, i );
				}
				break;
			}
		}
	}
}

// Для открывающей скобки с указанным индексом возвращает индекс соответствующей закрывающей скобки.
// Если закрывающая скобка не обнаружена возвращает -1.
int CNotationBuilder::findClosingBracket( int idx ) const
{
	std::stack< std::string > st;
	for( int j = 0; j < bracArraySize; ++j ) {
		if( bracket[j] == tokens[idx] ) {
			// К нам как-то попали не открывающиеся скобки.
			assert( j + bracArraySize / 2 < bracArraySize );

			st.push( bracket[j + bracArraySize / 2] );
		}
	}
	// К нам вообще попали не скобки.
	assert( !st.empty() );

	int i;
	for( i = idx + 1; i < static_cast<int>( tokens.size() ) && !st.empty(); ++i ) {
		int bracketId = -1;
		for( int j = 0; j < bracArraySize; ++j ) {
			if( bracket[j] == tokens[i] ) {
				bracketId = j;
			}
		}
		if( ( bracketId >= 0 ) && ( bracketId < bracArraySize / 2 ) ) {
			st.push( bracket[bracketId + bracArraySize / 2] );
		} else if( bracketId >= ( bracArraySize / 2 ) ) {
			// смотрим вершину стека на наличие скобки этого типа
			if( st.top() == tokens[i] ) {
				st.pop();
			} else {
				// Выдать ошибку - закрывающая скобка без пары
				throw CErrorCatcher( "Haven't opening bracket", tokens, i );
			}
		}
	}
	if( !st.empty() ) {
		return -1;
	} else {
		return i - 1;
	}
}

// Проверка синтаксиса для определенных операций - например sin требует аргумент в { } скобках.
bool CNotationBuilder::checkUnaryBracketsSyntax(  int idx, const std::string& expectedBracket ) const
{
	return  idx + 1 < static_cast<int>( tokens.size() ) ? tokens[idx + 1] == expectedBracket : false;
}

// Проверка синтаксиса для сложных операций, например sqrt[]{}.
bool CNotationBuilder::checkBinaryBracketsSyntax( int idx )
{
	if( currentToken == "\\sqrt" ) {
		if( idx + 1 >= static_cast<int>( tokens.size() ) || tokens[idx + 1] != "[") {
			throw CErrorCatcher( "Need [ after sqrt", tokens, idx );
		} else {
			int closingBracketIdx = findClosingBracket( idx + 1 );
			if( closingBracketIdx + 1 < static_cast<int>( tokens.size() ) && tokens[closingBracketIdx + 1] == "{" ) {
				currentToken = "\\sqrt[";
				return true;
			} else {
				// Ошибка - оператор sqrt[] не имеет значения для применения, ожидалось {.
				throw CErrorCatcher( "Need { after sqrt", tokens, idx );
			}
		}
	} else if( currentToken == "\\frac" ) {
		if( idx + 1 >= static_cast<int>( tokens.size() ) || tokens[idx + 1] != "{" ) {
			// Ошибка - оператор frac не имеет первого аргумента в формате {}, ожидалось {.
			throw CErrorCatcher( "Need { after frac", tokens, idx );
		} else {
			int closingBracketIdx = findClosingBracket( idx + 1 );
			if( closingBracketIdx + 1 < static_cast<int>( tokens.size() ) && tokens[closingBracketIdx + 1] == "{" ) {
				return true;
			} else {
				// ошибка - оператор frac не имеет второго аргумента в формате {}, ожидалось {.
				throw CErrorCatcher( "Haven't {", tokens, idx );
			}
		}
	} else {
		assert( false );
		return false;
	}
}

// Обработка связанная с закрытием скобки, то есть концом некоторого выражения.
void CNotationBuilder::processClosingBracket() {
	// Пока не встретили открывающуюся скобку, помещаем аргументы в дерево.
	// Здесь не может не быть скобки, т.к. 
	// мы ранее проверили корректность скобочный последовательности.
	while( !tempStack.empty() ) {
		if( tempStack.top().first == "(" ) {
			tempStack.pop();
			break;
		} else {
			outputNotation.push( tempStack.top() );
			tempStack.pop();
		}
	}

	// Если следом за скобкам в стеке оказалась унарная функция, значит это были ее аргументы.
	if( ( !tempStack.empty() ) && ( ( std::regex_match( tempStack.top().first, unaryFunctionRegex ) 
			|| ( std::regex_match( tempStack.top().first, unarySpecifiedFunctionRegex ) ) ) ) ) 
	{
		outputNotation.push( tempStack.top() );
		tempStack.pop();
	}
	mayUnary = false;
}

// Обработка бинарных операций/операторов.
void CNotationBuilder::processBinaryOperator() {
	if( mayUnary && ( currentToken == "-" ) ) {
		currentToken = "-#";
	}
	if( mayUnary && ( currentToken == "+") ) {
		currentToken = "+#";
	}
	while( !tempStack.empty() ) {
		bool isFunctionOrOperator = std::regex_match( tempStack.top().first, leftOpRegex ) || std::regex_match( tempStack.top().first, rightOpRegex ) || std::regex_match( tempStack.top().first, binaryFunctionRegex ) || std::regex_match( tempStack.top().first, agregationFunctionRegex );
		bool isPriorityCorrect = isFunctionOrOperator 
				&& ( ( ( std::regex_match( currentToken, leftOpRegex ) && operatorsPriority.find( tempStack.top().first )->second >= operatorsPriority.find( currentToken )->second ) 
				|| ( std::regex_match( currentToken, rightOpRegex ) && operatorsPriority.find( tempStack.top().first )->second > operatorsPriority.find( currentToken )->second ) ) );
		if( isFunctionOrOperator && isPriorityCorrect ) {
			outputNotation.push( tempStack.top() );
			tempStack.pop();
		} else {
			break;
		}
	}
	mayUnary = true;
}

// Некоторых токены должны быть положены в стек в самом конце и никак не связанным в данном контексте с другими.
// Выводим эти токены в дерево.
void CNotationBuilder::dumpToOutput() {
	while( !tempStack.empty() ) {
		outputNotation.push( tempStack.top() );
		tempStack.pop();
	}
	std::stack< std::pair<std::string, int>  > notation;
	while( !outputNotation.empty() ) {
		notation.push( outputNotation.top() );
		outputNotation.pop();
	}
	outputNotation = notation;
}
