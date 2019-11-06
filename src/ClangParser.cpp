
#include "ClangParser.h"

#include "ClangCursor.h"

#include "clang-c/Index.h"

#include <stdio.h>

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
int ClangParser::init()
{
	const char *compilerArgs[] = { "-xc++", "-fparse-all-comments" };

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
