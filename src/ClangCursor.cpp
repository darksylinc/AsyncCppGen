
#include "ClangCursor.h"

#include "ClangParser.h"

#include "AutoVars.h"

#include <assert.h>
#include <stdio.h>

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
ClangCursor::ClangCursor( CXCursor cursor, ClangCursor *parent, ClangParser *parser ) :
	mCursor( cursor ),
	mParent( parent ),
	mParser( parser ),
	mIsAsyncFunc( false )
{
}
//-------------------------------------------------------------------------
void ClangCursor::init()
{
	// printInfo();
	populateChildren();
	evaluate();
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
void ClangCursor::evaluate()
{
	std::string comments = toString( clang_Cursor_getRawCommentText( mCursor ) );

	const size_t asyncSwitchLength = sizeof( "!async_switch" ) - 1u;

	bool hasAsyncSwitch = false;
	size_t pos = comments.find( "!async_switch" );
	while( pos != std::string::npos )
	{
		const size_t classNameStart = pos + asyncSwitchLength + 1u;
		const size_t classNameEnd = comments.find_first_of( " ", classNameStart );

		const size_t memberVarNameStart = classNameEnd + 1u;
		const size_t memberVarNameEnd = comments.find_first_of( "\n", classNameEnd );

		std::string className = comments.substr( classNameStart, classNameEnd - classNameStart );
		std::string memberVarName =
			comments.substr( memberVarNameStart, memberVarNameEnd - memberVarNameStart );
		mParser->_addAsyncSwitchFunc( this, className, memberVarName );

		mIsAsyncFunc = true;
		hasAsyncSwitch = true;
		pos = comments.find( "!async_switch", memberVarNameEnd );
	}

	if( !hasAsyncSwitch && comments.find( "!async" ) != std::string::npos )
	{
		mIsAsyncFunc = true;
		mParser->_addAsyncFunc( this );
	}

	pos = comments.find( "!static_var" );
	if( pos != std::string::npos )
	{
		pos += sizeof( "!static_var" ) - 1u;
		AutoVars *autoVars = mParser->getAutoVars();
		autoVars->addStaticVar( comments, pos, this );
	}

	const size_t luaGfxBridgeLength = sizeof( "!lua_gfx_bridge" ) - 1u;
	pos = comments.find( "!lua_gfx_bridge" );
	if( pos != std::string::npos )
	{
		const size_t prefixNameStart = pos + luaGfxBridgeLength + 1u;
		const size_t prefixNameEnd = comments.find_first_of( " ", prefixNameStart );

		std::string prefixName;
		if( prefixNameStart < comments.size() )
			prefixName = comments.substr( prefixNameStart, prefixNameEnd - prefixNameStart );
		mParser->_addLuaGfxBridge( this, prefixName );
	}
}
//-------------------------------------------------------------------------
void ClangCursor::_addChild( CXCursor cursor, CXCursor parent )
{
	assert( parent.kind == mCursor.kind && parent.xdata == mCursor.xdata );
	mChildren.push_back( ClangCursor( cursor, this, mParser ) );
	mChildren.back().init();
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
CXTypeKind ClangCursor::getTypeKind() const
{
	return clang_getCursorType( mCursor ).kind;
}
//-------------------------------------------------------------------------
std::string ClangCursor::getTypeKindStr() const
{
	return toString( clang_getTypeKindSpelling( clang_getCursorType( mCursor ).kind ) );
}
//-------------------------------------------------------------------------
std::string ClangCursor::getTypeStr() const
{
	return toString( clang_getTypeSpelling( clang_getCursorType( mCursor ) ) );
}
//-------------------------------------------------------------------------
void ClangCursor::printInfo()
{
	printf( "%s %s %s\n", getKindStr().c_str(), getTypeStr().c_str(), getStr().c_str() );
	printf( "Comment %s\n", toString( clang_Cursor_getRawCommentText( mCursor ) ).c_str() );
	fflush( stdout );
}
