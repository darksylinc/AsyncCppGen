
#include "ClangParser.h"

#include "ClangCursor.h"

#include "clang-c/Index.h"

#include "fmt/format.h"

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
void ClangParser::loadFile( const char *filename, std::string &outString )
{
	std::ifstream inFile( filename, std::ios::in | std::ios::binary | std::ios::ate );
	const size_t fileSize = static_cast<size_t>( inFile.tellg() );
	outString.resize( fileSize );
	inFile.seekg( 0, std::ios::beg );
	inFile.read( &outString[0], (std::streamsize)fileSize );
}
//-------------------------------------------------------------------------
void ClangParser::saveFile( const char *filename, const std::string &text )
{
	std::ofstream inFile( filename, std::ios::out | std::ios::binary );
	inFile.write( text.c_str(), (std::streamsize)text.size() );
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
	const char *compilerArgs[] = { "-xc++", "-fparse-all-comments", "-I../Data/stdlib" };

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

	mRoot = new ClangCursor( cursor, 0, this );
	mRoot->init();

	return 0;
}
//-------------------------------------------------------------------------
void ClangParser::_addAsyncFunc( ClangCursor *cursorFunc )
{
	mAsyncFuncs.push_back( cursorFunc );
}
//-------------------------------------------------------------------------
void ClangParser::processAsyncFuncs()
{
	loadTemplates();

	std::string bodyHeader;
	std::string bodyCpp;

	ClangCursorPtrVec::const_iterator itor = mAsyncFuncs.begin();
	ClangCursorPtrVec::const_iterator endt = mAsyncFuncs.end();

	while( itor != endt )
	{
		processAsyncFunc( *itor, bodyHeader, bodyCpp );
		++itor;
	}

	bodyHeader = fmt::format( mFileBodyTemplate, "#pragma once", "MyNamespace", bodyHeader );
	saveFile( "./output.h", bodyHeader );
	bodyCpp = fmt::format( mFileBodyTemplate, "#include \"output.h\"", "MyNamespace", bodyCpp );
	saveFile( "./output.cpp", bodyCpp );
}
//-------------------------------------------------------------------------
void ClangParser::loadTemplates()
{
	loadFile( "../Data/Template01.cpp", mSourceClassTemplate );
	loadFile( "../Data/Template01.h", mHeaderClassTemplate );
	loadFile( "../Data/Template02.h", mFileBodyTemplate );
}
//-------------------------------------------------------------------------
void ClangParser::processAsyncFunc( ClangCursor *cursorFunc, std::string &bodyHeader,
									std::string &bodyCpp )
{
	std::string className = cursorFunc->getParent()->getStr();
	std::string funcName = cursorFunc->getStr();

	std::string headerVarDecl;
	std::string sourceFuncCopy;
	std::string varFuncDecl;
	std::string varFuncCall;

	const ClangCursorVec &children = cursorFunc->getChildren();

	std::string typeDecl;
	std::string varName;

	ClangCursorVec::const_iterator itor = children.begin();
	ClangCursorVec::const_iterator endt = children.end();

	while( itor != endt )
	{
		const ClangCursor &child = *itor;
		typeDecl.clear();
		varName.clear();
		std::string typeDecl = child.getTypeStr();
		std::string typeArgDecl = typeDecl;
		std::string varName = child.getStr();
		std::string varNameFuncCall = varName;

		if( typeDecl.back() == '&' )
		{
			// Convert references into hard copies
			typeDecl.pop_back();
		}
		else if( typeDecl == "const char *" )
		{
			// Convert string pointers into hard copies
			typeDecl = "std::string";
			typeArgDecl = "const char *";
			varNameFuncCall += ".c_str()";
		}

		// clang-format off
		headerVarDecl	+= typeDecl + " " + varName + ";\n";
		varFuncDecl		+= ", " + typeArgDecl + " _" + varName;
		sourceFuncCopy	+= varName + " = _" + varName + ";\n";
		varFuncCall		+= varNameFuncCall + ", ";
		// clang-format on
		++itor;
	}

	if( !varFuncCall.empty() )
	{
		varFuncCall.pop_back();
		varFuncCall.pop_back();
	}

	bodyHeader += fmt::format( mHeaderClassTemplate, className, funcName, headerVarDecl, varFuncDecl );
	bodyCpp += fmt::format( mSourceClassTemplate, className, funcName,  // {0}, {1}
							varFuncDecl,                                // {2}
							sourceFuncCopy,                             // {3}
							varFuncCall );                              // {4}
}
