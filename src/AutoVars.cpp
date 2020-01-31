
#include "AutoVars.h"

#include "ClangCursor.h"
#include "ClangParser.h"

#include "fmt/format.h"

AutoVars::AutoVars( ClangParser *clangParser ) : mClangParser( clangParser )
{
	ClangParser::loadFile( "../Data/AutoVars/Template01.cpp", mSourceAutoVarsTemplate );
}
//-------------------------------------------------------------------------
size_t AutoVars::extractParam( const std::string &comment, size_t pos, std::string &outStr )
{
	int depth = 0;
	bool charFound = false;

	std::string::const_iterator itor = comment.begin() + (int)pos;
	std::string::const_iterator endt = comment.end();
	while( itor != endt && *itor != '\n' &&
		   ( ( *itor != ' ' && *itor != '\t' ) || depth != 0 || !charFound ) )
	{
		if( *itor != ' ' && *itor != '\t' )
			charFound = true;
		if( *itor == '(' )
			++depth;
		if( *itor == ')' )
			--depth;
		outStr.push_back( *itor );
		++itor;
	}

	return static_cast<size_t>( itor - comment.begin() );
}
//-------------------------------------------------------------------------
void AutoVars::addStaticVar( const std::string &comment, size_t pos, ClangCursor *cursor )
{
	AutoVarEntry entry;
	pos = extractParam( comment, pos, entry.defaultValue );
	pos = extractParam( comment, pos, entry.minValue );
	pos = extractParam( comment, pos, entry.maxValue );
	pos = extractParam( comment, pos, entry.getter );
	pos = extractParam( comment, pos, entry.setter );
	entry.cursor = cursor;
	mAutoVars.push_back( entry );
}
//-------------------------------------------------------------------------
void AutoVars::setSettings( const std::string &outputSourceFullpath,
							const std::vector<std::string> &extraIncludesSource )
{
	mOutputSourceFullpath = outputSourceFullpath;

	mCustomIncludeSource.clear();
	std::vector<std::string>::const_iterator itor = extraIncludesSource.begin();
	std::vector<std::string>::const_iterator endt = extraIncludesSource.end();

	while( itor != endt )
	{
		mCustomIncludeSource += *itor;
		mCustomIncludeSource.push_back( '\n' );
		++itor;
	}
}
//-------------------------------------------------------------------------
void AutoVars::processAutoVars()
{
	if( mOutputSourceFullpath.empty() )
		return;

	std::string autoVarsCpp;

	AutoVarEntryVec::const_iterator itor = mAutoVars.begin();
	AutoVarEntryVec::const_iterator endt = mAutoVars.end();

	while( itor != endt )
	{
		ClangCursor *cursor = itor->cursor;
		std::string className = cursor->getParent()->getStr();
		std::string varName = cursor->getStr();
		std::string varType = cursor->getTypeStr();

		autoVarsCpp +=
			fmt::format( "{0} {1}::{2} = {3};\n", varType, className, varName, itor->defaultValue );
		++itor;
	}

	autoVarsCpp = fmt::format( mSourceAutoVarsTemplate, mCustomIncludeSource,
							   mClangParser->getCustomNamespace(), autoVarsCpp );

	mClangParser->saveFile( mOutputSourceFullpath.c_str(), autoVarsCpp );
}
