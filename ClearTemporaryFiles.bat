@echo off
setlocal enabledelayedexpansion

REM Folders to delete
set folders=Build IrisEditor\Build IrisEngine\Build .vs out

for %%d in (%folders%) do (
    if exist "%%d" (
        echo Deleting %%d...
        rmdir /s /q "%%d"
    ) else (
        echo Folder %%d does not exist.
    )
)

echo Cleanup Done.
pause