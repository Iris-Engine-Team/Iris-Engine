#include "Core/Tools/Profiler.h"

namespace Iris
{
	namespace Debug
	{ 
        std::unordered_map<std::type_info*, int> Profiler::s_NbCopyObj;
        std::unordered_map<std::type_info*, int> Profiler::s_NbMoveObj;
        std::unordered_map<void*, std::function<void(void)>> Profiler::s_AllocatedObj;

        void Profiler::AddCopy(std::type_info* _type)
        {
            if (s_NbCopyObj.find(_type) != s_NbCopyObj.end())
                ++s_NbCopyObj[_type];

            else
                s_NbCopyObj[_type] = 1;
        }

        void Profiler::AddMove(std::type_info* _type)
        {
            if (s_NbMoveObj.find(_type) != s_NbMoveObj.end())
                ++s_NbMoveObj[_type];

            else
                s_NbMoveObj[_type] = 1;
        }

        void Profiler::RemoveMemoryAlloc(void* _pointer)
        {
            s_AllocatedObj.erase(_pointer);
        }

        void Profiler::RemoveAllMemoryAlloc()
        {
            for (auto& alloc : s_AllocatedObj)
                alloc.second();

            s_AllocatedObj.clear();
        }
	}
}
