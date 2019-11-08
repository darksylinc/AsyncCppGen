
#pragma once

#include "clang-c/Index.h"

#include <string>
#include <vector>

class ClangParser;
class ClangCursor;
typedef std::vector<ClangCursor> ClangCursorVec;

class ClangCursor
{
	CXCursor mCursor;
	ClangCursor *mParent;
	ClangCursorVec mChildren;

	ClangParser *mParser;

	bool mIsAsyncFunc;

	void populateChildren();

	void evaluate();

public:
	ClangCursor( CXCursor cursor, ClangCursor *parent, ClangParser *parser );

	void init();

	ClangCursor *getParent() const { return mParent; }
	const ClangCursorVec &getChildren() const { return mChildren; }

	void _addChild( CXCursor cursor, CXCursor parent );

	static std::string toString( CXString cxString );

	std::string getStr() const;

	CXCursorKind getKind() const;
	std::string getKindStr() const;

	CXTypeKind getTypeKind() const;
	/// Pretty much useless
	std::string getTypeKindStr() const;
	/// Useful, pretty prints the type of variable
	std::string getTypeStr() const;

	void printInfo();
};
