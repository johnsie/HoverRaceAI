@echo off
setlocal enabledelayedexpansion

set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
set "RELEASE_DIR=C:\originalhr2\HoverRaceAI\Release"
set "VIDEO_PROJ=C:\originalhr2\HoverRaceAI\NetTarget\VideoServices\VideoServices.vcxproj"
set "GAME_PROJ=C:\originalhr2\HoverRaceAI\NetTarget\Game2\Game2.vcxproj"

echo.
echo ======================================
echo Build Starting - Clean Release Build
echo ======================================
echo.

REM Kill any running Game2 process
taskkill /IM Game2.exe /F /T >nul 2>&1

REM Build VideoServices
echo Building VideoServices...
cd /d "C:\originalhr2\HoverRaceAI\NetTarget\VideoServices"
"!VS_PATH!" "!VIDEO_PROJ!" /p:Configuration=Release /p:Platform=Win32 /p:OutDir="!RELEASE_DIR!\" /v:minimal
if errorlevel 1 (
    echo ERROR: VideoServices build failed
    exit /b 1
)

REM Copy SDL2
echo Copying SDL2.dll...
copy "C:\SDL2\lib\x86\SDL2.dll" "!RELEASE_DIR!\SDL2.dll" /Y >nul 2>&1

REM Build Game2
echo Building Game2...
cd /d "C:\originalhr2\HoverRaceAI\NetTarget\Game2"
"!VS_PATH!" "!GAME_PROJ!" /p:Configuration=Release /p:Platform=Win32 /p:OutDir="!RELEASE_DIR!\" /v:minimal
if errorlevel 1 (
    echo ERROR: Game2 build failed
    exit /b 1
)

echo.
echo ======================================
echo Build Complete - Checking Files
echo ======================================
echo.

dir "!RELEASE_DIR!\Game2.exe" 2>nul
dir "!RELEASE_DIR!\VideoServices.dll" 2>nul

echo.
echo Ready to run Game2.exe
pause
