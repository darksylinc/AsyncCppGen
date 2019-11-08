
#include "ClangParser.h"

#include "fmt/core.h"

int main()
{
	ClangParser parser;

	int result = parser.init();
	if( result )
		return result;

	parser.processAsyncFuncs();

	return 0;
}
