@echo off
setlocal enabledelayedexpansion

REM Build script that writes output directly to file to avoid terminal blocking

cd /d C:\originalhr2\HoverRaceAI

REM Write start marker
echo [BUILD_START] %date% %time% > C:\originalhr2\HoverRaceAI\build_status.txt

REM Run MSBuild and capture output
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
    NetTarget\Game2\Game2.vcxproj ^
    /t:Rebuild ^
    /p:Configuration=Release ^
    /p:Platform=Win32 ^
    /v:minimal >> C:\originalhr2\HoverRaceAI\build_status.txt 2>&1

REM Check result
set BUILD_RESULT=%ERRORLEVEL%
echo [BUILD_EXIT_CODE] %BUILD_RESULT% >> C:\originalhr2\HoverRaceAI\build_status.txt

if %BUILD_RESULT% EQU 0 (
    echo [BUILD_SUCCESS] >> C:\originalhr2\HoverRaceAI\build_status.txt
    for /f "tokens=*" %%A in ('dir /b C:\originalhr2\HoverRaceAI\Release\Game2.exe') do (
        echo [OUTPUT_FILE] %%A >> C:\originalhr2\HoverRaceAI\build_status.txt
    )
) else (
    echo [BUILD_FAILED] >> C:\originalhr2\HoverRaceAI\build_status.txt
)

echo [BUILD_END] %date% %time% >> C:\originalhr2\HoverRaceAI\build_status.txt

exit /b %BUILD_RESULT%
