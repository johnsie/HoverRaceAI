@echo off
REM MazeCompiler Batch Wrapper
REM Automatically recompiles tracks with correct palette
REM 
REM Usage:
REM   compile_tracks.bat
REM
REM This script:
REM   1. Ensures ColorTab.pal is in the current directory
REM   2. Uses HoverCad debug files from %TEMP% to recompile tracks
REM   3. Generates new .TRK files with current palette embedded

setlocal enabledelayedexpansion

echo ====================================
echo HoverRace Track Recompilation Script
echo ====================================
echo.

REM Configuration
set MAZE_COMPILER="%CD%\MazeCompiler.exe"
set COLOR_TAB="%CD%\ColorTab.pal"
set OUTPUT_DIR="%CD%\Tracks\mazes"

REM Check if MazeCompiler exists
if not exist %MAZE_COMPILER% (
    echo ERROR: MazeCompiler.exe not found in current directory
    echo Current directory: %CD%
    pause
    exit /b 1
)

echo MazeCompiler found: %MAZE_COMPILER%
echo.

REM Check if ColorTab.pal exists
if not exist %COLOR_TAB% (
    echo WARNING: ColorTab.pal not found in current directory
    echo Looking in ..\NetTarget\bitmaps\...
    
    if exist "..\NetTarget\bitmaps\ColorTab.pal" (
        echo Found! Copying to current directory...
        copy "..\NetTarget\bitmaps\ColorTab.pal" %COLOR_TAB%
        if errorlevel 1 (
            echo ERROR: Failed to copy ColorTab.pal
            pause
            exit /b 1
        )
    ) else (
        echo ERROR: ColorTab.pal not found anywhere
        echo Please ensure ColorTab.pal is in: %CD%
        pause
        exit /b 1
    )
)

echo ColorTab.pal verified in: %COLOR_TAB%
echo.

REM Check if output directory exists
if not exist %OUTPUT_DIR% (
    echo Creating output directory: %OUTPUT_DIR%
    mkdir %OUTPUT_DIR%
)

echo Output directory: %OUTPUT_DIR%
echo.

REM Look for HoverCad debug files in TEMP
echo Looking for HoverCad debug files in: %TEMP%
echo.

setlocal enabledelayedexpansion
set count=0

for %%F in ("%TEMP%\HC*.tmp.debug") do (
    set /a count+=1
    
    echo Processing: %%~nF
    
    REM Extract track name (remove HC prefix and .tmp.debug suffix)
    set filename=%%~nF
    set trackname=!filename:~2,-10!
    set output="%OUTPUT_DIR%\!trackname!.trk"
    
    echo   Output: !output!
    
    REM Copy debug file to temp txt file
    set txtfile="!TEMP!\temp_track.txt"
    copy "%%F" !txtfile! > nul
    
    REM Run MazeCompiler
    echo   Compiling...
    %MAZE_COMPILER% !output! !txtfile!
    
    if errorlevel 1 (
        echo   [FAILED]
    ) else (
        echo   [OK]
    )
    
    REM Clean up temp txt file
    if exist !txtfile! del !txtfile!
    
    echo.
)

if %count% equ 0 (
    echo No HoverCad debug files found in %TEMP%
    echo.
    echo You need to:
    echo   1. Open tracks in HoverCad: C:\originalhr\HoverRace\HoverCad\Release\HoverCad.exe
    echo   2. Click the Compile button for each track
    echo   3. This will create HC*.tmp.debug files in %%TEMP%%
    echo   4. Run this script again
    echo.
    pause
    exit /b 1
)

echo ====================================
echo Recompilation complete!
echo Check %OUTPUT_DIR% for compiled .trk files
echo ====================================
echo.
pause
