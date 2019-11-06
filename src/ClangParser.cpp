
#include "ClangParser.h"

#include "ClangCursor.h"

#include "clang-c/Index.h"

#include <stdio.h>

#include <fstream>

ClangParser::ClangParser() : mIndex( 0 ), mUnit( 0 ), mRoot( 0 )
{
}
//-------------------------------------------------------------------------
ClangParser::~ClangParser()
{
	delete mRoot;
	mRoot = 0;

	if( mUnit )
	{
		clang_disposeTranslationUnit( mUnit );
		mUnit = 0;
	}
	if( mIndex )
	{
		clang_disposeIndex( mIndex );
		mIndex = 0;
	}
}
//-------------------------------------------------------------------------
void ClangParser::initUnsavedFiles( const char **filenames, size_t numFilenames )
{
	mUnsavedFiles.reserve( mUnsavedFiles.size() + numFilenames );
	for( size_t i = 0u; i < numFilenames; ++i )
	{
		mUnsavedFiles.push_back( UnsavedFile() );
		initUnsavedFile( filenames[i], mUnsavedFiles.back() );
	}
}
//-------------------------------------------------------------------------
void ClangParser::initUnsavedFile( const char *filename, UnsavedFile &outUnsavedFile )
{
	std::ifstream inFile( filename, std::ios::in | std::ios::binary | std::ios::ate );
	const size_t fileSize = static_cast<size_t>( inFile.tellg() );
	outUnsavedFile.filename = filename;
	outUnsavedFile.contents.resize( fileSize + 1u );
	inFile.seekg( 0, std::ios::beg );
	inFile.read( &outUnsavedFile.contents[0], (std::streamsize)fileSize );
	outUnsavedFile.contents[fileSize] = 0;  // Always null terminated
}
//-------------------------------------------------------------------------
std::vector<CXUnsavedFile> ClangParser::getCXUnsavedFiles() const
{
	std::vector<CXUnsavedFile> retVal;
	retVal.reserve( mUnsavedFiles.size() );

	std::vector<UnsavedFile>::const_iterator itor = mUnsavedFiles.begin();
	std::vector<UnsavedFile>::const_iterator endt = mUnsavedFiles.end();

	while( itor != endt )
	{
		CXUnsavedFile unsavedFile;
		unsavedFile.Filename = itor->filename.c_str();
		unsavedFile.Length = itor->contents.size();
		unsavedFile.Contents = &itor->contents[0];
		retVal.push_back( unsavedFile );
		++itor;
	}

	return retVal;
}
//-------------------------------------------------------------------------
int ClangParser::init()
{
	// Provide a path to a fake std lib implementation to avoid cluttering std vector & string
	const char *compilerArgs[] = { "-xc++", "-fparse-all-comments",
								   "-I../Data/stdlib" };

	//	const char *stdlibStub[] = { "../Data/stdlib/string",
	//								 "../Data/stdlib/vector" };
	//	initUnsavedFiles( stdlibStub, sizeof( stdlibStub ) / sizeof( stdlibStub[0] ) );
	//	std::vector<CXUnsavedFile> unsavedFiles = getCXUnsavedFiles();

	mIndex = clang_createIndex( 0, true );
	mUnit = clang_parseTranslationUnit(
		mIndex, "/home/matias/Projects/AsyncCppGen/bin/Data/example.h", compilerArgs,
		sizeof( compilerArgs ) / sizeof( compilerArgs[0] ), 0, 0,
		CXTranslationUnit_None | CXTranslationUnit_IncludeBriefCommentsInCodeCompletion );

	if( !mUnit )
	{
		printf( "Unable to parse translation unit. Quitting.\n" );
		return -1;
	}

	CXCursor cursor = clang_getTranslationUnitCursor( mUnit );

	mRoot = new ClangCursor( cursor, 0 );

	return 0;
}
