
#pragma once

#include "clang-c/Index.h"

#include <string>
#include <vector>

class ClangCursor;
typedef std::vector<ClangCursor> ClangCursorVec;

class ClangCursor
{
	CXCursor mCursor;
	ClangCursor *mParent;
	ClangCursorVec mChildren;

	void populateChildren();

public:
	ClangCursor( CXCursor cursor, ClangCursor *parent );

	void _addChild( CXCursor cursor, CXCursor parent );

	static std::string toString( CXString cxString );

	std::string getStr() const;

	CXCursorKind getKind() const;

	std::string getKindStr() const;

	void printInfo();
};
