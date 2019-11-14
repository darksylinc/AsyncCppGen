
#include "ClangParser.h"

#include "fmt/core.h"

void printHelp()
{
	printf( "Usage:\n" );
	printf( "    AsyncCppGen /path/to/config.ini\n" );
}

void getValuesFromIni( const std::string &inIniData, const char *keyName,
					   std::vector<std::string> &outValues )
{
	size_t pos = inIniData.find( keyName );
	while( pos != std::string::npos )
	{
		const size_t startPos = inIniData.find_first_of( '=', pos );
		const size_t endPos = inIniData.find_first_of( '\n', startPos );
		std::string newVal = inIniData.substr( startPos + 1u, endPos - ( startPos + 1u ) );
		outValues.push_back( newVal );
		pos = inIniData.find( keyName, endPos );
	}
}

std::string getValueFromIni( const std::string &inIniData, const char *keyName )
{
	std::string retVal;

	size_t pos = inIniData.find( keyName );
	if( pos != std::string::npos )
	{
		const size_t startPos = inIniData.find_first_of( '=', pos );
		const size_t endPos = inIniData.find_first_of( '\n', startPos );
		retVal = inIniData.substr( startPos + 1u, endPos - ( startPos + 1u ) );
	}
	else
	{
		printf( "Setting %s not found!", keyName );
		exit( -1 );
	}

	return retVal;
}

int main( int argn, char *argv[] )
{
	if( argn != 2 )
	{
		printHelp();
		return 0;
	}

	std::string iniContent;
	ClangParser::loadFile( argv[1], iniContent );

	if( iniContent.empty() )
	{
		printf( "Error could not open %s file not found or empty", argv[1] );
		return -1;
	}

	ClangParser parser;

	std::vector<std::string> extraIncludesHeader;
	getValuesFromIni( iniContent, "extra_include_header", extraIncludesHeader );
	std::vector<std::string> extraIncludesSource;
	getValuesFromIni( iniContent, "extra_include_source", extraIncludesSource );
	parser.setSettings( getValueFromIni( iniContent, "namespace" ),      //
						getValueFromIni( iniContent, "macro_prefix" ),   //
						getValueFromIni( iniContent, "output_header" ),  //
						getValueFromIni( iniContent, "output_source" ),  //
						extraIncludesHeader, extraIncludesSource );

	std::vector<std::string> includeFolders;
	getValuesFromIni( iniContent, "compiler_arg", includeFolders );

	int result = parser.init( getValueFromIni( iniContent, "file_to_parse" ).c_str(), includeFolders );
	if( result )
		return result;

	parser.processAsyncFuncs();

	return 0;
}
