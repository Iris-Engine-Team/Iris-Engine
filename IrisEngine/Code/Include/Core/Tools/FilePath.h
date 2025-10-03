#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"

#define IRIS_EDITOR "IrisEngine"

#define NUM_ARGS(...)				\
[](bool a, ...)						\
{									\
    va_list vl;						\
    va_start(vl, a);				\
    char* val;						\
    int nb = 0;						\
    while (val = va_arg(vl, char*))	\
        ++nb;						\
									\
    return nb;						\
}(true, __VA_ARGS__, nullptr)

IE_FORCE_INLINE std::string RecursivePath(std::filesystem::path root, const std::string& target)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(root))
	{
		if (entry.path().filename() == target)
		{
			//std::cout << entry.path().string() << '\n';
			return entry.path().string();
		}
	}
	DEBUG_WARN(("File " + target + " not found!").c_str());
	return "";
}

#ifdef _WIN32
#include <windows.h>
IE_FORCE_INLINE std::filesystem::path GetExecutablePath()
{
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	return std::filesystem::path(path).parent_path();
}
#else
#include <unistd.h>
IE_FORCE_INLINE std::string GetExecutablePath()
{
	char path[1024];
	ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
	return std::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path().string();
}
#endif

IE_FORCE_INLINE std::filesystem::path FindPath(std::string targetFile = IRIS_EDITOR)
{
	return RecursivePath(GetExecutablePath().parent_path().parent_path(), targetFile);
}

#define FIND_PATH(filePath) FindPath(filePath)
