// Автор: Орлов Никита.

#include <Utils.h>
#include <regex>

std::vector<std::string> ConsturctTokens( const std::string& src )
{
	std::vector<std::string> tokens;

	std::string pattern = "([0-9]+\\.[0-9]*)|(\\.[0-9]+)";
	pattern += "|([0-9]*)|([\\*\\-+/\\^_=])|(\\\\((frac)|(sqrt)|(sum)|(sin)|(cos)|(tg)|(ctg)|(le)|(ge)|(ne)|(vee)|(wedge)))|([\\{\\}\\[\\]\\(\\)])";
	pattern += "|(\\\\[a-z]*)|(,)|([a-z]*)|([<>])";

	std::tr1::regex separatorPattern( pattern );

	std::tr1::sregex_token_iterator token( src.begin(), src.end(), separatorPattern, 0 );
	std::tr1::sregex_token_iterator endOfTokenizing;

	while( token != endOfTokenizing ) {
		if( *token != "" ) {
			tokens.emplace_back( *token++ );
		} else {
			token++;
		}
	}
	return tokens;
}

bool IsAggregationFunction( const std::string& src )
{
	return src == "\\sum" || src == "\\mul";
}