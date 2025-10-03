add_subdirectory(Externals)

option(IRIS_ENGINE_UT_WINDOW "Enable unit test for window" OFF)
option(IRIS_ENGINE_UT_APLLICATION "Enable unit test for application" OFF)
option(IRIS_ENGINE_UT_SECS "Enable unit test for scene entity component system" OFF)

if(IRIS_ENGINE_UT_WINDOW)
	add_subdirectory(UT_Window)
endif()

if(IRIS_ENGINE_UT_APPLICATION)
	add_subdirectory(UT_Application)
endif()

if(IRIS_ENGINE_UT_SECS)
	add_subdirectory(UT_SECS)
endif()