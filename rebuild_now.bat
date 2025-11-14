@echo off
setlocal enabledelayedexpansion

set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
set "RELEASE_DIR=C:\originalhr2\HoverRaceAI\Release"
set "VIDEO_PROJ=C:\originalhr2\HoverRaceAI\NetTarget\VideoServices\VideoServices.vcxproj"
set "GAME_PROJ=C:\originalhr2\HoverRaceAI\NetTarget\Game2\Game2.vcxproj"

cls
echo.
echo ======================================
echo Clean Rebuild - Starting
echo ======================================
echo.

taskkill /IM Game2.exe /F /T >nul 2>&1
echo - Game process stopped

if exist "!RELEASE_DIR!" (
    echo - Cleaning Release directory...
    rmdir /S /Q "!RELEASE_DIR!" >nul 2>&1
    timeout /t 1 >nul
)

mkdir "!RELEASE_DIR!" >nul 2>&1
echo - Created Release directory

echo.
echo Building VideoServices.dll...
cd /d "C:\originalhr2\HoverRaceAI\NetTarget\VideoServices"
"!VS_PATH!" "!VIDEO_PROJ!" /p:Configuration=Release /p:Platform=Win32 /p:OutDir="!RELEASE_DIR!\" /v:minimal
if errorlevel 1 (
    echo ERROR: VideoServices build failed!
    pause
    exit /b 1
)
echo - VideoServices built successfully

echo Copying SDL2.dll...
copy "C:\SDL2\lib\x86\SDL2.dll" "!RELEASE_DIR!\SDL2.dll" /Y >nul 2>&1
echo - SDL2.dll copied

echo.
echo Building Game2.exe...
cd /d "C:\originalhr2\HoverRaceAI\NetTarget\Game2"
"!VS_PATH!" "!GAME_PROJ!" /p:Configuration=Release /p:Platform=Win32 /p:OutDir="!RELEASE_DIR!\" /v:minimal
if errorlevel 1 (
    echo ERROR: Game2 build failed!
    pause
    exit /b 1
)
echo - Game2 built successfully

echo.
echo ======================================
echo Clean Rebuild Complete
echo ======================================
echo.
echo Files in Release:
dir "!RELEASE_DIR!" /B
echo.
echo Ready to run Game2.exe
pause
