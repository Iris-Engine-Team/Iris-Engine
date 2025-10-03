#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"

#include <unordered_map>
#include <typeinfo>
#include <functional>


namespace Iris
{
    namespace Debug
    {
        class Profiler
        {
        private:
            static std::unordered_map<std::type_info*, int> s_NbCopyObj;
            static std::unordered_map<std::type_info*, int> s_NbMoveObj;
            static std::unordered_map<void*, std::function<void(void)>> s_AllocatedObj;

        public:
            IE_FORCE_INLINE static const std::unordered_map<std::type_info*, int>& GetCopyObj() { return s_NbCopyObj; }
            IE_FORCE_INLINE static const std::unordered_map<std::type_info*, int>& GetMoveObj() { return s_NbMoveObj; }
            IE_FORCE_INLINE static const std::unordered_map<void*, std::function<void(void)>>& GetAllocatedObj() { return s_AllocatedObj; }

            static void AddCopy(std::type_info* _type);
            static void AddMove(std::type_info* _type);
            template <typename T>
            static void AddMemoryAlloc(T* _pointer)
            {
                s_AllocatedObj[_pointer] = [=]() { delete _pointer; };
            }
            static void RemoveMemoryAlloc(void* _pointer);
            static void RemoveAllMemoryAlloc();
            IE_FORCE_INLINE static void PrintMemoryAllocState()
            {
                size_t nbAllocs = s_AllocatedObj.size();

                if (nbAllocs)
                    DEBUG_WARN("%i allocated objects find", static_cast<int>(nbAllocs));
            }
        };
    }
}