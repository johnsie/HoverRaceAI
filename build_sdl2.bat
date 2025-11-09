@echo off
REM SDL2 Graphics Integration - Build Test Script
REM This script helps verify that the SDL2/OpenGL integration is working correctly

setlocal enabledelayedexpansion

echo.
echo ========================================================
echo SDL2 Graphics Integration - Build Test
echo ========================================================
echo.

REM Check for Visual Studio
set "MSBUILD_PATH="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    echo [✓] Found VS 2022 Community
) else if exist "C:\Program Files\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
    echo [✓] Found VS 2019 Community
) else (
    echo [✗] Visual Studio not found in standard locations
    echo Please ensure Visual Studio is installed
    exit /b 1
)

echo.
echo Checking project files...

REM Check if projects exist
if not exist "NetTarget.sln" (
    echo [✗] NetTarget.sln not found
    echo Please run this script from the HoverRace root directory
    exit /b 1
)

echo [✓] NetTarget.sln found
echo.

REM Check if SDL2 integration files exist
echo Checking SDL2 integration files...

set "MISSING_FILES=0"

if not exist "NetTarget\GraphicsSDL2\SDL2Graphics.h" (
    echo [✗] Missing: NetTarget\GraphicsSDL2\SDL2Graphics.h
    set "MISSING_FILES=1"
) else (
    echo [✓] Found: SDL2Graphics.h
)

if not exist "NetTarget\GraphicsSDL2\SDL2Graphics.cpp" (
    echo [✗] Missing: NetTarget\GraphicsSDL2\SDL2Graphics.cpp
    set "MISSING_FILES=1"
) else (
    echo [✓] Found: SDL2Graphics.cpp
)

if not exist "NetTarget\GraphicsSDL2\VideoBufferSDL2Adapter.h" (
    echo [✗] Missing: VideoBufferSDL2Adapter.h
    set "MISSING_FILES=1"
) else (
    echo [✓] Found: VideoBufferSDL2Adapter.h
)

if not exist "NetTarget\GraphicsSDL2\VideoBufferSDL2Adapter.cpp" (
    echo [✗] Missing: VideoBufferSDL2Adapter.cpp
    set "MISSING_FILES=1"
) else (
    echo [✓] Found: VideoBufferSDL2Adapter.cpp
)

if not exist "NetTarget\VideoServices\VideoBufferSDL2Integration.h" (
    echo [✗] Missing: VideoBufferSDL2Integration.h
    set "MISSING_FILES=1"
) else (
    echo [✓] Found: VideoBufferSDL2Integration.h
)

if not exist "NetTarget\VideoServices\VideoBufferSDL2Integration.cpp" (
    echo [✗] Missing: VideoBufferSDL2Integration.cpp
    set "MISSING_FILES=1"
) else (
    echo [✓] Found: VideoBufferSDL2Integration.cpp
)

if !MISSING_FILES! equ 1 (
    echo.
    echo [✗] Some files are missing
    exit /b 1
)

echo.
echo All integration files present!
echo.

REM Ask for build configuration
echo Configuration options:
echo   1. Clean and Rebuild Release (Recommended for first build)
echo   2. Clean and Rebuild Debug (Better for troubleshooting)
echo   3. Quick Rebuild Release (Faster, incremental)
echo   4. Just Clean (Don't build)
echo.
set /p BUILD_OPTION="Select option (1-4): "

if "!BUILD_OPTION!"=="1" (
    echo.
    echo Building Release configuration (Clean + Rebuild)...
    echo.
    "!MSBUILD_PATH!" NetTarget.sln /p:Configuration=Release /p:Platform=x64 /t:Clean /t:Rebuild
    if !ERRORLEVEL! equ 0 (
        echo.
        echo [✓] Build completed successfully!
        exit /b 0
    ) else (
        echo.
        echo [✗] Build failed with errors
        exit /b 1
    )
) else if "!BUILD_OPTION!"=="2" (
    echo.
    echo Building Debug configuration (Clean + Rebuild)...
    echo.
    "!MSBUILD_PATH!" NetTarget.sln /p:Configuration=Debug /p:Platform=x64 /t:Clean /t:Rebuild
    if !ERRORLEVEL! equ 0 (
        echo.
        echo [✓] Build completed successfully!
        exit /b 0
    ) else (
        echo.
        echo [✗] Build failed with errors
        exit /b 1
    )
) else if "!BUILD_OPTION!"=="3" (
    echo.
    echo Building Release configuration (Incremental rebuild)...
    echo.
    "!MSBUILD_PATH!" NetTarget.sln /p:Configuration=Release /p:Platform=x64 /t:Rebuild
    if !ERRORLEVEL! equ 0 (
        echo.
        echo [✓] Build completed successfully!
        exit /b 0
    ) else (
        echo.
        echo [✗] Build failed with errors
        exit /b 1
    )
) else if "!BUILD_OPTION!"=="4" (
    echo.
    echo Cleaning solution...
    echo.
    "!MSBUILD_PATH!" NetTarget.sln /p:Configuration=Release /p:Platform=x64 /t:Clean
    if !ERRORLEVEL! equ 0 (
        echo.
        echo [✓] Clean completed successfully!
        exit /b 0
    ) else (
        echo.
        echo [✗] Clean failed with errors
        exit /b 1
    )
) else (
    echo.
    echo [✗] Invalid option
    exit /b 1
)
