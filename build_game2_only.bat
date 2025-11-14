@echo off
cd /d C:\originalhr2\HoverRaceAI
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" NetTarget\Game2\Game2.vcxproj /p:Configuration=Release /p:Platform=Win32 > build_game2.log 2>&1
if exist Release\Game2.exe (
  echo Build SUCCESS
  dir Release\Game2.exe
) else (
  echo Build FAILED
  echo Last 50 lines of log:
  powershell -Command "Get-Content build_game2.log | Select-Object -Last 50"
)
