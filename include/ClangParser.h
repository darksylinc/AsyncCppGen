
#pragma once

#include <map>
#include <string>
#include <vector>

typedef void *CXIndex;
typedef struct CXTranslationUnitImpl *CXTranslationUnit;

struct CXUnsavedFile;

class ClangCursor;

struct SwitchAsyncFunc
{
	std::string memberVariableName;
	ClangCursor *cursor;
};

typedef std::vector<ClangCursor *> ClangCursorPtrVec;

typedef std::vector<SwitchAsyncFunc> SwitchAsyncFuncVec;
typedef std::map<std::string, SwitchAsyncFuncVec> SwitchAsyncFuncVecMap;

class ClangParser
{
	struct UnsavedFile
	{
		std::string filename;
		std::vector<char> contents;
	};

	CXIndex mIndex;

	std::vector<CXTranslationUnit> mUnits;
	std::vector<ClangCursor *> mRoots;

	std::vector<UnsavedFile> mUnsavedFiles;

	ClangCursorPtrVec mAsyncFuncs;
	SwitchAsyncFuncVecMap mAsyncSwitchFuncs;

	std::string mHeaderClassTemplate;
	std::string mSourceClassTemplate;
	std::string mFileBodySourceTemplate;
	std::string mFileBodyHeaderTemplate;

	std::string mSourceAsyncSwitchTemplateFuncBody;
	std::string mSourceAsyncSwitchTemplateCaseBody;
	std::string mSourceAsyncSwitchTemplateClassDecl;
	std::string mHeaderAsyncSwitchTemplateClassDecl;

	std::string mCustomNamespace;
	std::string mCustomMacroPrefix;
	std::string mCustomIncludeHeader;
	std::string mCustomIncludeSource;

	std::string mOutputHeaderFullpath;
	std::string mOutputSourceFullpath;

	std::vector<CXUnsavedFile> getCXUnsavedFiles() const;

public:
	/// Loads the contents of 'filename' into outString
	static void loadFile( const char *filename, std::string &outString );

protected:
	/// Saves the contents of text into 'filename'
	static void saveFile( const char *filename, const std::string &text );

	void initUnsavedFiles( const char **filenames, size_t numFilenames );
	static void initUnsavedFile( const char *filename, UnsavedFile &outUnsavedFile );

	/// Loads all templates (e.g. mHeaderClassTemplate, mSourceClassTemplate & mFileBodyTemplate)
	/// from data folder
	void loadTemplates();

	/// Formats a particular async function
	void processAsyncFunc( ClangCursor *cursorFunc, std::string &bodyHeader, std::string &bodyCpp );

	/// Formats a particular async switch function
	void processAsyncSwitchFunc( ClangCursor *cursorFunc, const std::string &className,
								 const std::string &memberVarName, size_t internalIdx,
								 std::string &bodyHeader, std::string &bodyCpp,
								 std::string &switchBodyCpp );

public:
	ClangParser();
	~ClangParser();

	int init( const std::vector<std::string> &pathToFilesToParse,
			  const std::vector<std::string> &includeFolders );

	void _addAsyncFunc( ClangCursor *cursorFunc );
	void _addAsyncSwitchFunc( ClangCursor *cursorFunc, const std::string &className,
							  const std::string &memberVarName );

	void setSettings( const std::string &namespaceValue, const std::string &macroPrefix,
					  const std::string &outputHeaderFullpath, const std::string &outputSourceFullpath,
					  const std::vector<std::string> &extraIncludesHeader,
					  const std::vector<std::string> &extraIncludesSource );

	/// Iterates all async functions, formats them and generates final output
	void processAsyncFuncs();
};
