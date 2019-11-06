
#pragma once

#include <string>
#include <vector>

typedef void *CXIndex;
typedef struct CXTranslationUnitImpl *CXTranslationUnit;

struct CXUnsavedFile;

class ClangCursor;

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

	std::vector<CXUnsavedFile> getCXUnsavedFiles() const;

	void initUnsavedFiles( const char **filenames, size_t numFilenames );
	static void initUnsavedFile( const char *filename, UnsavedFile &outUnsavedFile );

public:
	ClangParser();
	~ClangParser();

	int init();
};
