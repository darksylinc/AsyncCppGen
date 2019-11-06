
#pragma once

typedef void *CXIndex;
typedef struct CXTranslationUnitImpl *CXTranslationUnit;

class ClangCursor;

class ClangParser
{
	CXIndex mIndex;
	CXTranslationUnit mUnit;

	ClangCursor *mRoot;

public:
	ClangParser();
	~ClangParser();

	int init();
};
