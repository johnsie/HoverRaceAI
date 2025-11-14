@echo off
setlocal

set RELEASE_DIR=C:\originalhr2\HoverRaceAI\Release
set SDL2_SRC=C:\SDL2\lib\x86\SDL2.dll

echo Copying SDL2...
copy "%SDL2_SRC%" "%RELEASE_DIR%\SDL2.dll" /Y
if errorlevel 1 goto err_sdl2
echo SDL2 copied

echo.
echo Building Game2...
cd /d C:\originalhr2\HoverRaceAI\NetTarget\Game2
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Game2.vcxproj /p:Configuration=Release /p:Platform=Win32 /v:minimal
if errorlevel 1 goto err_game2
echo Game2 built

echo.
echo === CLEAN REBUILD COMPLETE ===
echo.
dir "%RELEASE_DIR%\Game2.exe" "%RELEASE_DIR%\VideoServices.dll" "%RELEASE_DIR%\SDL2.dll" 2>nul
echo.
pause
goto :eof

:err_sdl2
echo ERROR: SDL2 copy failed
pause
exit /b 1

:err_game2
echo ERROR: Game2 build failed
pause
exit /b 1
