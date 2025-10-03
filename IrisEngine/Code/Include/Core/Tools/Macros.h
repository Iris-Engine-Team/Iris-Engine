#pragma once

#include <string>
#include <filesystem>
#include <stdarg.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX


#ifdef _MSC_VER
// MSVC compiler (Visual Studio): Forces inline with __forceinline
#define IE_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
// GCC/Clang compilers: Forces inline with always_inline
#define IE_FORCE_INLINE __attribute__((always_inline)) inline
#else
// Other compilers: Default to standard inline
#define IE_FORCE_INLINE inline
#endif

#ifdef IRIS_ENGINE_EXPORTS
	#define IRIS_ENGINE_API __declspec(dllexport)
#else
	#define IRIS_ENGINE_API __declspec(dllimport)
#endif