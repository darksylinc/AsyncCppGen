
#pragma once

#include <string>
#include <vector>

typedef void *CXIndex;
typedef struct CXTranslationUnitImpl *CXTranslationUnit;

struct CXUnsavedFile;

class ClangCursor;
typedef std::vector<ClangCursor *> ClangCursorPtrVec;

class ClangParser
{
	struct UnsavedFile
	{
		std::string filename;
		std::vector<char> contents;
	};

	CXIndex mIndex;
	CXTranslationUnit mUnit;

	ClangCursor *mRoot;

	std::vector<UnsavedFile> mUnsavedFiles;

	ClangCursorPtrVec mAsyncFuncs;

	std::string mHeaderClassTemplate;
	std::string mSourceClassTemplate;
	std::string mFileBodySourceTemplate;
	std::string mFileBodyHeaderTemplate;

	std::string mCustomNamespace;
	std::string mCustomMacroPrefix;
	std::string mCustomIncludeHeader;

	std::vector<CXUnsavedFile> getCXUnsavedFiles() const;

	/// Loads the contents of 'filename' into outString
	void loadFile( const char *filename, std::string &outString );
	/// Saves the contents of text into 'filename'
	void saveFile( const char *filename, const std::string &text );

	void initUnsavedFiles( const char **filenames, size_t numFilenames );
	static void initUnsavedFile( const char *filename, UnsavedFile &outUnsavedFile );

	/// Loads all templates (e.g. mHeaderClassTemplate, mSourceClassTemplate & mFileBodyTemplate)
	/// from data folder
	void loadTemplates();

	/// Formats a particular async function
	void processAsyncFunc( ClangCursor *cursorFunc, std::string &bodyHeader, std::string &bodyCpp );

public:
	ClangParser();
	~ClangParser();

	int init( const char *pathToFileToParse, const std::vector<std::string> &includeFolders );

	void _addAsyncFunc( ClangCursor *cursorFunc );

	/// Iterates all async functions, formats them and generates final output
	void processAsyncFuncs();
};
