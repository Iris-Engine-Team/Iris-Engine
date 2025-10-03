# Requires C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Root
set(JOLT_PHYSICS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE JOLT_PHYSICS_SRC_FILES 
"*.cpp"
)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
	# Add natvis file
	set(JOLT_PHYSICS_SRC_FILES ${JOLT_PHYSICS_SRC_FILES} ${JOLT_PHYSICS_ROOT}/Jolt.natvis)
endif()

# Group source files
source_group(TREE ${JOLT_PHYSICS_ROOT} FILES ${JOLT_PHYSICS_SRC_FILES})

# Create Jolt lib
add_library(Jolt ${JOLT_PHYSICS_SRC_FILES})
add_library(Jolt::Jolt ALIAS Jolt)

target_include_directories(Jolt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if (BUILD_SHARED_LIBS)
	# Set default visibility to hidden
	set(CMAKE_CXX_VISIBILITY_PRESET hidden)

	if (GENERATE_DEBUG_SYMBOLS)
		if (MSVC)
			# MSVC specific option to enable PDB generation
			set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG:FASTLINK")
		else()
			# Clang/GCC option to enable debug symbol generation
			set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -g")
		endif()
	endif()

	# Set linker flags for other build types to be the same as release
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASEASAN "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASEUBSAN "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASETSAN "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASECOVERAGE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
	set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTION "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")

	# Public define to instruct user code to import Jolt symbols (rather than use static linking)
	target_compile_definitions(Jolt PUBLIC JPH_SHARED_LIBRARY)

	# Private define to instruct the library to export symbols for shared linking
	target_compile_definitions(Jolt PRIVATE JPH_BUILD_SHARED_LIBRARY)
endif()

# Use repository as include directory when building, install directory when installing
target_include_directories(Jolt PUBLIC
	$<BUILD_INTERFACE:${PHYSICS_REPO_ROOT}>
	$<INSTALL_INTERFACE:include/>)

# Code coverage doesn't work when using precompiled headers
if (CMAKE_GENERATOR STREQUAL "Ninja Multi-Config" AND MSVC)
	# The Ninja Multi-Config generator errors out when selectively disabling precompiled headers for certain configurations.
	# See: https://github.com/jrouwe/JoltPhysics/issues/1211
	target_precompile_headers(Jolt PRIVATE "${JOLT_PHYSICS_ROOT}/Jolt/Jolt.h")
else()
	target_precompile_headers(Jolt PRIVATE "$<$<NOT:$<CONFIG:ReleaseCoverage>>:${JOLT_PHYSICS_ROOT}/Jolt/Jolt.h>")
endif()

if (NOT CPP_EXCEPTIONS_ENABLED)
	# Disable use of exceptions in MSVC's STL
	target_compile_definitions(Jolt PUBLIC $<$<BOOL:${MSVC}>:_HAS_EXCEPTIONS=0>)
endif()

# Set the debug/non-debug build flags
target_compile_definitions(Jolt PUBLIC "$<$<CONFIG:Debug>:_DEBUG>")
target_compile_definitions(Jolt PUBLIC "$<$<CONFIG:Release,Distribution,ReleaseASAN,ReleaseUBSAN,ReleaseTSAN,ReleaseCoverage>:NDEBUG>")

# ASAN and TSAN should use the default allocators
target_compile_definitions(Jolt PUBLIC "$<$<CONFIG:ReleaseASAN,ReleaseTSAN>:JPH_DISABLE_TEMP_ALLOCATOR;JPH_DISABLE_CUSTOM_ALLOCATOR>")

# Setting floating point exceptions
if (FLOATING_POINT_EXCEPTIONS_ENABLED AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_definitions(Jolt PUBLIC "$<$<CONFIG:Debug,Release>:JPH_FLOATING_POINT_EXCEPTIONS_ENABLED>")
endif()

# Setting the disable custom allocator flag
if (DISABLE_CUSTOM_ALLOCATOR)
	target_compile_definitions(Jolt PUBLIC JPH_DISABLE_CUSTOM_ALLOCATOR)
endif()

# Setting enable asserts flag
if (USE_ASSERTS)
	target_compile_definitions(Jolt PUBLIC JPH_ENABLE_ASSERTS)
endif()

# Setting double precision flag
if (DOUBLE_PRECISION)
	target_compile_definitions(Jolt PUBLIC JPH_DOUBLE_PRECISION)
endif()

# Setting to attempt cross platform determinism
if (CROSS_PLATFORM_DETERMINISTIC)
	target_compile_definitions(Jolt PUBLIC JPH_CROSS_PLATFORM_DETERMINISTIC)
endif()

# Setting to determine number of bits in ObjectLayer
if (OBJECT_LAYER_BITS)
	target_compile_definitions(Jolt PUBLIC JPH_OBJECT_LAYER_BITS=${OBJECT_LAYER_BITS})
endif()

if (USE_STD_VECTOR)
	target_compile_definitions(Jolt PUBLIC JPH_USE_STD_VECTOR)
endif()

# Setting to periodically trace broadphase stats to help determine if the broadphase layer configuration is optimal
if (TRACK_BROADPHASE_STATS)
	target_compile_definitions(Jolt PUBLIC JPH_TRACK_BROADPHASE_STATS)
endif()

# Setting to periodically trace narrowphase stats to help determine which collision queries could be optimized
if (TRACK_NARROWPHASE_STATS)
	target_compile_definitions(Jolt PUBLIC JPH_TRACK_NARROWPHASE_STATS)
endif()

# Enable the debug renderer
if (DEBUG_RENDERER_IN_DISTRIBUTION)
	target_compile_definitions(Jolt PUBLIC "JPH_DEBUG_RENDERER")
elseif (DEBUG_RENDERER_IN_DEBUG_AND_RELEASE)
	target_compile_definitions(Jolt PUBLIC "$<$<CONFIG:Debug,Release,ReleaseASAN,ReleaseUBSAN,ReleaseTSAN>:JPH_DEBUG_RENDERER>")
endif()

# Enable the profiler
if (PROFILER_IN_DISTRIBUTION)
	target_compile_definitions(Jolt PUBLIC "JPH_PROFILE_ENABLED")
elseif (PROFILER_IN_DEBUG_AND_RELEASE)
	target_compile_definitions(Jolt PUBLIC "$<$<CONFIG:Debug,Release,ReleaseASAN,ReleaseUBSAN,ReleaseTSAN>:JPH_PROFILE_ENABLED>")
endif()

# Compile the ObjectStream class and RTTI attribute information
if (ENABLE_OBJECT_STREAM)
	target_compile_definitions(Jolt PUBLIC JPH_OBJECT_STREAM)
endif()

# Emit the instruction set definitions to ensure that child projects use the same settings even if they override the used instruction sets (a mismatch causes link errors)
function(EMIT_X86_INSTRUCTION_SET_DEFINITIONS)
	if (USE_AVX512)
		target_compile_definitions(Jolt PUBLIC JPH_USE_AVX512)
	endif()
	if (USE_AVX2)
		target_compile_definitions(Jolt PUBLIC JPH_USE_AVX2)
	endif()
	if (USE_AVX)
		target_compile_definitions(Jolt PUBLIC JPH_USE_AVX)
	endif()
	if (USE_SSE4_1)
		target_compile_definitions(Jolt PUBLIC JPH_USE_SSE4_1)
	endif()
	if (USE_SSE4_2)
		target_compile_definitions(Jolt PUBLIC JPH_USE_SSE4_2)
	endif()
	if (USE_LZCNT)
		target_compile_definitions(Jolt PUBLIC JPH_USE_LZCNT)
	endif()
	if (USE_TZCNT)
		target_compile_definitions(Jolt PUBLIC JPH_USE_TZCNT)
	endif()
	if (USE_F16C)
		target_compile_definitions(Jolt PUBLIC JPH_USE_F16C)
	endif()
	if (USE_FMADD AND NOT CROSS_PLATFORM_DETERMINISTIC)
		target_compile_definitions(Jolt PUBLIC JPH_USE_FMADD)
	endif()
endfunction()

# Add the compiler commandline flags to select the right instruction sets
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	if ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86" OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		if (USE_AVX512)
			target_compile_options(Jolt PUBLIC /arch:AVX512)
		elseif (USE_AVX2)
			target_compile_options(Jolt PUBLIC /arch:AVX2)
		elseif (USE_AVX)
			target_compile_options(Jolt PUBLIC /arch:AVX)
		endif()
		EMIT_X86_INSTRUCTION_SET_DEFINITIONS()
	endif()
else()
	if (XCODE)
		# XCode builds for multiple architectures, we can't set global flags
	elseif (CROSS_COMPILE_ARM OR CMAKE_OSX_ARCHITECTURES MATCHES "arm64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
		# ARM64 uses no special commandline flags
	elseif (EMSCRIPTEN)
		if (USE_WASM_SIMD)
			# Jolt currently doesn't implement the WASM specific SIMD intrinsics so uses the SSE 4.2 intrinsics
			# See: https://emscripten.org/docs/porting/simd.html#webassembly-simd-intrinsics
			# Note that this does not require the browser to actually support SSE 4.2 it merely means that it can translate those instructions to WASM SIMD instructions
			target_compile_options(Jolt PUBLIC -msimd128 -msse4.2)
		endif()
		if (JPH_USE_WASM64)
			target_compile_options(Jolt PUBLIC -sMEMORY64)
			target_link_options(Jolt PUBLIC -sMEMORY64)
		endif()
	elseif ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i386")
		# x86 and x86_64
		# On 32-bit builds we need to default to using SSE instructions, the x87 FPU instructions have higher intermediate precision
		# which will cause problems in the collision detection code (the effect is similar to leaving FMA on, search for
		# JPH_PRECISE_MATH_ON for the locations where this is a problem).

		if (USE_AVX512)
			target_compile_options(Jolt PUBLIC -mavx512f -mavx512vl -mavx512dq -mavx2 -mbmi -mpopcnt -mlzcnt -mf16c)
		elseif (USE_AVX2)
			target_compile_options(Jolt PUBLIC -mavx2 -mbmi -mpopcnt -mlzcnt -mf16c)
		elseif (USE_AVX)
			target_compile_options(Jolt PUBLIC -mavx -mpopcnt)
		elseif (USE_SSE4_2)
			target_compile_options(Jolt PUBLIC -msse4.2 -mpopcnt)
		elseif (USE_SSE4_1)
			target_compile_options(Jolt PUBLIC -msse4.1)
		else()
			target_compile_options(Jolt PUBLIC -msse2)
		endif()
		if (USE_LZCNT)
			target_compile_options(Jolt PUBLIC -mlzcnt)
		endif()
		if (USE_TZCNT)
			target_compile_options(Jolt PUBLIC -mbmi)
		endif()
		if (USE_F16C)
			target_compile_options(Jolt PUBLIC -mf16c)
		endif()
		if (USE_FMADD AND NOT CROSS_PLATFORM_DETERMINISTIC)
			target_compile_options(Jolt PUBLIC -mfma)
		endif()

		if (NOT MSVC)
			target_compile_options(Jolt PUBLIC -mfpmath=sse)
		endif()

		EMIT_X86_INSTRUCTION_SET_DEFINITIONS()
	endif()
endif()

# On Unix flavors we need the pthread library
if (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows") AND NOT EMSCRIPTEN)
	target_compile_options(Jolt PUBLIC -pthread)
	target_link_options(Jolt PUBLIC -pthread)
endif()

if (EMSCRIPTEN)
	# We need more than the default 64KB stack and 16MB memory
	# In your application, specify at least -sSTACK_SIZE=1048576 -sINITIAL_MEMORY=134217728
	# Also disable warning: running limited binaryen optimizations because DWARF info requested (or indirectly required)
	target_link_options(Jolt PUBLIC -Wno-limited-postlink-optimizations)
endif()
