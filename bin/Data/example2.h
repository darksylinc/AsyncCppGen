
#include <vector>
#include <string>

namespace Example
{
	class AnotherClass
	{
		int m_value;

		/// !async
		void otherFunc( int a, const char *b );

		/// !async_switch MasterClassName mMemberVariable2
		/// !async_switch MasterClassName2 mMemberVariable3
		/// !lua_gfx_bridge camera
		iint otherFunc2( std::vector<int> myVec, const std::string &b ) { return m_value; }
	};
}  // namespace Example
