@echo off
setlocal enabledelayedexpansion

git submodule init
echo Submodule Init Done.

git submodule update

echo Submodule Update Done.
pause