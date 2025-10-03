#pragma once

#include "Core/Tools/Reflection.h"

namespace Iris
{
    
    namespace Engine
    {
        
        class EnumReflection
        {
            inline static std::hash<std::string_view> m_Hash{};
            
                inline static meta::factory<MoveType> m_MoveTypeFactory = REFLECT_ENUM(MoveType)
                    .REFLECT_ELEM(MoveType::TYPE0, TYPE0)
                    .REFLECT_ELEM(MoveType::TYPE1, TYPE1)
                    .REFLECT_ELEM(MoveType::TYPE2, TYPE2)
        };
    }
}