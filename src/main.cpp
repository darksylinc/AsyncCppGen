
#include "ClangParser.h"

int main()
{
	ClangParser parser;

	int result = parser.init();
	if( result )
		return result;

	return 0;
}
