@echo off
cd /d C:\originalhr2\HoverRaceAI
echo Rebuilding Game2.exe...
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" NetTarget\Game2\Game2.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32 /v:minimal
if %ERRORLEVEL% EQU 0 (
    echo.
    echo SUCCESS: Game2.exe rebuilt
    dir Release\Game2.exe
) else (
    echo.
    echo FAILED: Build returned error %ERRORLEVEL%
)
pause
