@echo off
REM Verification script for MazeCompiler fix
REM Tests if the level serialization bug has been fixed

setlocal enabledelayedexpansion

cd /d "C:\originalhr\HoverRace\Release" || exit /b 1

echo.
echo ========================================
echo MazeCompiler Fix Verification
echo ========================================
echo.

REM Step 1: Check if MazeCompiler.exe exists
if not exist "MazeCompiler.exe" (
    echo ERROR: MazeCompiler.exe not found
    exit /b 1
)
echo [OK] MazeCompiler.exe found

REM Step 2: Check if debug file exists
if not exist "%TEMP%\HCA972.tmp.debug" (
    echo ERROR: Debug file not found at %%TEMP%%\HCA972.tmp.debug
    echo Please open a track in HoverCad and click compile to create it
    exit /b 1
)
echo [OK] Debug file found at %%TEMP%%\HCA972.tmp.debug

REM Step 3: Run MazeCompiler with test output
echo.
echo Running MazeCompiler with test file...
echo.

REM Delete old test file if exists
if exist "Tracks\ClassicH_test.trk" del "Tracks\ClassicH_test.trk"

REM Run MazeCompiler and capture output
MazeCompiler.exe Tracks\ClassicH_test.trk "%TEMP%\HCA972.tmp.debug" > mazecompiler_test.log 2>&1

REM Check if output file was created
if exist "Tracks\ClassicH_test.trk" (
    echo [OK] Output file created: Tracks\ClassicH_test.trk
    
    REM Get file size
    for %%A in (Tracks\ClassicH_test.trk) do set "size=%%~zA"
    echo     File size: !size! bytes
    
    REM Compare with original
    if exist "Tracks\ClassicH.trk" (
        for %%A in (Tracks\ClassicH.trk) do set "orig_size=%%~zA"
        echo     Original size: !orig_size! bytes
        
        if !size! GTR !orig_size! (
            echo [OK] New file is LARGER (contains level geometry)
        ) else (
            echo [WARNING] New file is NOT larger - may not contain level geometry
        )
    )
) else (
    echo [ERROR] Output file NOT created
)

REM Step 4: Check log for key messages
echo.
echo Checking compilation log...
echo.

findstr /i "Level serialization completed" mazecompiler_test.log > nul
if !errorlevel! equ 0 (
    echo [OK] Found: "Level serialization completed successfully"
    echo [SUCCESS] Level geometry is being serialized
) else (
    echo [ERROR] Did NOT find "Level serialization completed successfully"
    
    findstr /i "Skipping Serialize due to stability" mazecompiler_test.log > nul
    if !errorlevel! equ 0 (
        echo [ERROR] Found: "Skipping Serialize due to stability issues"
        echo [FAILURE] Still using OLD buggy code - MazeCompiler was not rebuilt
    ) else (
        echo [WARNING] Cannot determine serialization status
    )
)

REM Step 5: Show relevant log output
echo.
echo Log file excerpt:
echo ============================================
findstr /i "BeginANewRecord\|Serialize\|Level serialization\|ERROR: Unable to add" mazecompiler_test.log
echo ============================================
echo.

REM Step 6: Show full log
echo.
echo Full compilation log (mazecompiler_test.log):
echo ============================================
type mazecompiler_test.log
echo ============================================

echo.
echo [VERIFICATION COMPLETE]
echo.
echo Next steps:
echo 1. If [SUCCESS] shown above: The fix is working
echo 2. Replace ClassicH.trk with the test file
echo 3. Run Game2.exe and test track loading
echo.
pause
