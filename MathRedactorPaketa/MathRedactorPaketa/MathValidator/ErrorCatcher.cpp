// Автор: Тимур Хусаенов.

#include <ErrorCatcher.h>
#include <algorithm>

CErrorCatcher::CErrorCatcher( std::string message, const std::vector<std::string>& tokens, int index ) : msg(message), subStr("")
{
	if( index == -1 ) {
		subStr = "all formula";
		return;
	}

	for( int i = std::max(0, index - 10); i < std::min( static_cast<int>( tokens.size() ), index + 10 ); ++i ) {
		subStr += tokens[i];
	}
}

