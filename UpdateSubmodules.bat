@echo off
setlocal enabledelayedexpansion

for %%D in (
    "IrisEngine/Externals/SPIRV-Reflect"
    "IrisEngine/Externals/assimp"
    "IrisEngine/Externals/stb"
    "IrisEngine/Externals/glfw"
    "IrisEngine/Externals/IrisEngine_Math"
    "IrisEngine/Externals/json"
    "IrisEngine/Externals/meta"
) do (
    if not exist %%D (
        mkdir %%D
        echo Created: %%D
    ) else (
        echo Already exists: %%D
    )
)

git submodule init
echo Submodule Init Done.

git submodule update

echo Submodule Update Done.
pause