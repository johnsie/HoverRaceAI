@echo off
REM Force copy with explicit paths
set SRC=C:\originalhr2\HoverRaceAI\Debug
set DST=C:\originalhr2\HoverRaceAI\Release

echo Source: %SRC%
echo Destination: %DST%
echo.

if exist "%SRC%\MainCharacter.dll" (
    echo Found MainCharacter.dll, copying...
    copy /Y "%SRC%\MainCharacter.dll" "%DST%\MainCharacter.dll"
    if exist "%DST%\MainCharacter.dll" (
        echo SUCCESS: MainCharacter.dll copied
    ) else (
        echo FAILED: MainCharacter.dll not in destination
    )
) else (
    echo ERROR: MainCharacter.dll not found in source
)

echo.
dir "%DST%\MainCharacter.dll" 2>&1
