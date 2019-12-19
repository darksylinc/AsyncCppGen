
#include <vector>
#include <string>

namespace Example
{
	class AnotherClass
	{
		/// !async
		void otherFunc( int a, const char *b );

		/// !async_switch MasterClassName mMemberVariable2
		/// !async_switch MasterClassName2 mMemberVariable3
		void otherFunc2( std::vector<int> myVec, const std::string &b );
	};
}  // namespace Example
