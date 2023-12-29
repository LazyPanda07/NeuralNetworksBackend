#include "Utils.h"

#include <filesystem>

#include "Sample.h"

namespace utility
{
	std::string getPathToCurrentModule()
	{
		return std::filesystem::path(pathToCurrentModule).parent_path().string();
	}
}
