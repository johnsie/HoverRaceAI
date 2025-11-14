@echo off
REM Clean build batch file for HoverRace
REM Usage: build.cmd [clean|build|run]

setlocal enabledelayedexpansion

if "%1"=="clean" (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_clean.ps1" -SkipClean $false
    goto :eof
)

if "%1"=="build" (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_clean.ps1" -SkipClean $false -BuildOnly $false
    goto :eof
)

if "%1"=="run" (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_clean.ps1" -SkipClean $true -RunGame $true
    goto :eof
)

if "%1"=="quick" (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_clean.ps1" -SkipClean $true
    goto :eof
)

REM Default: Full clean build
echo.
echo HoverRace Build System
echo ======================
echo Usage:
echo   build.cmd         - Full clean and build (default)
echo   build.cmd clean   - Clean only
echo   build.cmd build   - Build without cleaning
echo   build.cmd quick   - Build without cleaning
echo   build.cmd run     - Run the game (assumes build is current)
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_clean.ps1"
