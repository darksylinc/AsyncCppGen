
#include <vector>
#include <string>

namespace Example
{
	class MyClass
	{
		/// !async
		void myFunc( int a, const char *b );

		/// !async
		void myFunc2( std::vector<int> myVec, const std::string &b );

		/// !async_switch MasterClassName mMemberVariable0
		/// !async_switch MasterClassName2 mMemberVariable1
		void myFunc3( std::vector<int> myVec, const std::string &b );
	};
}  // namespace Example
