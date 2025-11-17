@echo off
setlocal enabledelayedexpansion

cd /d C:\originalhr2\HoverRaceAI

echo Build started at %date% %time%
echo.

"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
    NetTarget\Game2\Game2.vcxproj ^
    /t:Rebuild ^
    /p:Configuration=Release ^
    /p:Platform=Win32 ^
    /v:minimal

set BUILD_RESULT=%ERRORLEVEL%

echo.
echo Build completed with exit code: %BUILD_RESULT%
if %BUILD_RESULT% EQU 0 (
    echo SUCCESS!
    dir Release\Game2.exe
) else (
    echo FAILED - See errors above
)

exit /b %BUILD_RESULT%
