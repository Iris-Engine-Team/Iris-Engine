@echo off
REM Folder for the solution
set BUILD_DIR=IrisProject

call "%~dp0UpdateSubmodules.bat"

REM Deletes the old build folder
if exist %BUILD_DIR% (
    rmdir /s /q %BUILD_DIR%
)

REM Creates a new build folder
mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Generates the visual studio solution
cmake -G "Visual Studio 17 2022" ..

echo Visual Studio 2022 solution generated

echo Compiling cmake build

cmake --build . --config Release

echo Cmake build compiled

pause