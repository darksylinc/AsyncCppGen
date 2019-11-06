
#include "ClangCursor.h"

#include <stdio.h>
#include <assert.h>

static CXChildVisitResult cxCursorCountChildren( CXCursor, CXCursor, CXClientData clientData )
{
	size_t *numChildren = (size_t *)clientData;
	++( *numChildren );
	return CXChildVisit_Continue;
}

static CXChildVisitResult cxCursorCollectChildren( CXCursor cursor, CXCursor parent,
												   CXClientData clientData )
{
	ClangCursor *clangCursor = (ClangCursor *)clientData;
	clangCursor->_addChild( cursor, parent );
	return CXChildVisit_Continue;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
ClangCursor::ClangCursor( CXCursor cursor, ClangCursor *parent ) : mCursor( cursor ), mParent( parent )
{
	populateChildren();
}
//-------------------------------------------------------------------------
void ClangCursor::populateChildren()
{
	size_t numChildren = 0u;
	clang_visitChildren( mCursor, ::cxCursorCountChildren, &numChildren );
	mChildren.reserve( numChildren );
	clang_visitChildren( mCursor, ::cxCursorCollectChildren, this );
}
//-------------------------------------------------------------------------
void ClangCursor::_addChild( CXCursor cursor, CXCursor parent )
{
	assert( parent.kind == mCursor.kind && parent.xdata == mCursor.xdata &&
			parent.data[0] == mCursor.data[0] );
	mChildren.push_back( ClangCursor( cursor, this ) );
}
//-------------------------------------------------------------------------
std::string ClangCursor::toString( CXString cxString )
{
	std::string retVal;
	const char *cStr = clang_getCString( cxString );
	if( cStr )
		retVal = cStr;
	clang_disposeString( cxString );
	return retVal;
}
//-------------------------------------------------------------------------
std::string ClangCursor::getStr() const
{
	return toString( clang_getCursorSpelling( mCursor ) );
}
//-------------------------------------------------------------------------
CXCursorKind ClangCursor::getKind() const
{
	return clang_getCursorKind( mCursor );
}
//-------------------------------------------------------------------------
std::string ClangCursor::getKindStr() const
{
	return toString( clang_getCursorKindSpelling( getKind() ) );
}
//-------------------------------------------------------------------------
void ClangCursor::printInfo()
{
	printf( "type %s name %s\n", getKindStr().c_str(), getStr().c_str() );
	printf( "Comment %s\n", toString( clang_Cursor_getRawCommentText( mCursor ) ).c_str() );
}
