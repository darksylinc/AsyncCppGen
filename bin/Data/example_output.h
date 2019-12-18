
#include <vector>
#include <string>
//#include "vector"
//#include "string"

namespace Example
{
	class MyClass
	{
	public:
		/// !async
		void myFunc( int a, const char *b );

		/// !async
		void myFunc2( std::vector<int> myVec, const std::string b );

		int resultFetch( double a ) const;
	};

	class AsyncMyClass_myFunc
	{
		MyClass *dst;
		int a;
		std::string b;
	};

}  // namespace Example
