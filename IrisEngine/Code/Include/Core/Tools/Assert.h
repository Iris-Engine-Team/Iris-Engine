#pragma once
#include <exception>
#include <iostream>
#include <string_view>

#if _DEBUG
#define DEBUG_Assert(assertion) if(assertion) {} else __debugbreak();
#elif NDEBUG
#define DEBUG_Assert(assertion) if(assertion) {} else abort();
#endif