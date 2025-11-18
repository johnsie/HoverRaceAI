@echo off
setlocal enabledelayedexpansion

REM Build Game2
echo Building Game2...
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" NetTarget\Game2\Game2.vcxproj /p:Configuration=Release /p:Platform=Win32 /v:minimal

REM Check if exe was created
if exist "NetTarget\Game2\Release\Game2.exe" (
    echo Game2.exe created successfully
    copy "NetTarget\Game2\Release\Game2.exe" "Release\Game2.exe"
    if exist "Release\Game2.exe" (
        echo Game2.exe copied to Release folder
        dir "Release\Game2.exe"
    ) else (
        echo ERROR: Failed to copy Game2.exe
    )
) else (
    echo ERROR: Game2.exe not found in NetTarget\Game2\Release\
    dir "NetTarget\Game2\Release\"
)

pause
