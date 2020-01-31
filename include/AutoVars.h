
#pragma once

#include <string>
#include <vector>

class ClangParser;
class ClangCursor;

struct AutoVarEntry
{
	std::string defaultValue;
	std::string maxValue;
	std::string minValue;
	std::string getter;
	std::string setter;
	ClangCursor *cursor;
};

typedef std::vector<AutoVarEntry> AutoVarEntryVec;

class AutoVars
{
	AutoVarEntryVec mAutoVars;

	std::string mSourceAutoVarsTemplate;

	std::string mCustomIncludeSource;

	std::string mOutputSourceFullpath;

	ClangParser *mClangParser;

	size_t extractParam( const std::string &comment, size_t pos, std::string &outStr );

public:
	AutoVars( ClangParser *clangParser );

	void addStaticVar( const std::string &comment, size_t pos, ClangCursor *cursor );

	void setSettings( const std::string &outputSourceFullpath,
					  const std::vector<std::string> &extraIncludesSource );

	void processAutoVars();
};
