@echo off
setlocal enabledelayedexpansion

set "MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
set "RELEASE_DIR=c:\originalhr2\HoverRaceAI\Release"

echo Killing Game2...
taskkill /IM Game2.exe /F /T >nul 2>&1

echo Deleting old Game2.exe...
del "%RELEASE_DIR%\Game2.exe" /F /Q 2>nul

echo Building VideoServices...
"!MSBUILD!" "c:\originalhr2\HoverRaceAI\NetTarget\VideoServices\VideoServices.vcxproj" /p:Configuration=Release /p:Platform=Win32 /t:Rebuild
if errorlevel 1 goto error

echo Building Game2...
"!MSBUILD!" "c:\originalhr2\HoverRaceAI\NetTarget\Game2\Game2.vcxproj" /p:Configuration=Release /p:Platform=Win32 /t:Rebuild
if errorlevel 1 goto error

echo Build complete!
dir "%RELEASE_DIR%\Game2.exe" /T:W
goto done

:error
echo ERROR: Build failed!
exit /b 1

:done
