
#include "ClangParser.h"

#include "fmt/core.h"

int main()
{
	ClangParser parser;

	std::vector<std::string> includeFolders = {
		"-I/home/matias/Projects/Vidya/Scripts/AsyncCppGen",
//		"-I/home/matias/Projects/Vidya/include",
//		"-I/home/matias/Projects/Vidya/Dependencies/Ogre/OgreMain/include",
//		"-I/home/matias/Projects/Vidya/Dependencies/Ogre/build/Debug/include",
//		"-I/home/matias/Projects/Vidya/Dependencies/Ogre/Components/Overlay/include",
//		"-I/usr/include/SDL2"
	};

	// "/home/matias/Projects/AsyncCppGen/bin/Data/example.h"
	int result = parser.init( "/home/matias/Projects/Vidya/include/Graphics/VidyaGraphicsSystem.h",
							  includeFolders );
	if( result )
		return result;

	parser.processAsyncFuncs();

	return 0;
}
