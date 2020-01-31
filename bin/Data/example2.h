
#include <vector>
#include <string>

namespace Example
{
	class AnotherClass
	{
		/// !auto_vars 78
		int m_value;

		/// !auto_vars 1.0f -1.0f 5.0f
		float m_autoVarTest;

		/// !async
		void otherFunc( int a, const char *b );

		/// !async_switch MasterClassName mMemberVariable2
		/// !async_switch MasterClassName2 mMemberVariable3
		/// !lua_gfx_bridge camera
		int otherFunc2( std::vector<int> myVec, const std::string &b ) { return m_value; }

		/// !async_switch MasterClassName mMemberVariable2
		/// !async_switch MasterClassName2 mMemberVariable3
		/// !lua_gfx_bridge
		int otherFunc3( std::vector<int> myVec, const std::string &b ) { return m_value; }
	};
}  // namespace Example
