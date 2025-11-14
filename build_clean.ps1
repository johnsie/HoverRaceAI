# Comprehensive clean build script for HoverRace
# Removes all intermediate and output files before rebuilding

param(
    [switch]$SkipClean = $false,
    [switch]$BuildOnly = $false,
    [switch]$RunGame = $false
)

$ErrorActionPreference = "Stop"

# Configuration
$RootDir = "C:\originalhr2\HoverRaceAI"
$NetTargetDir = "$RootDir\NetTarget"
$ReleaseDir = "$RootDir\Release"
$MSBuildExe = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
$SDL2Dll = "C:\SDL2\lib\x86\SDL2.dll"

Write-Host "================================" -ForegroundColor Cyan
Write-Host "HoverRace Clean Build System" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# Kill running game processes
Write-Host "Stopping any running Game2 processes..." -ForegroundColor Yellow
Stop-Process -Name "Game2" -ErrorAction SilentlyContinue -Force
Start-Sleep -Milliseconds 500

if (-not $SkipClean) {
    Write-Host ""
    Write-Host "===== CLEANUP PHASE =====" -ForegroundColor Yellow
    
    # Kill any locked file handles
    Write-Host "Releasing file locks..." -ForegroundColor Gray
    Get-Process | Where-Object { $_.Name -like "*Game2*" } | Stop-Process -Force -ErrorAction SilentlyContinue
    Start-Sleep -Milliseconds 300
    
    # Remove Release directory completely
    Write-Host "Removing Release directory..." -ForegroundColor Gray
    if (Test-Path $ReleaseDir) {
        Remove-Item -Path $ReleaseDir -Recurse -Force -ErrorAction SilentlyContinue
        Start-Sleep -Milliseconds 200
    }
    
    # Clean VideoServices project
    Write-Host "Cleaning VideoServices intermediate files..." -ForegroundColor Gray
    $VideoServicesRelease = "$NetTargetDir\VideoServices\Release"
    if (Test-Path $VideoServicesRelease) {
        Remove-Item -Path $VideoServicesRelease -Recurse -Force -ErrorAction SilentlyContinue
    }
    
    # Clean Game2 project
    Write-Host "Cleaning Game2 intermediate files..." -ForegroundColor Gray
    $Game2Release = "$NetTargetDir\Game2\Release"
    if (Test-Path $Game2Release) {
        Remove-Item -Path $Game2Release -Recurse -Force -ErrorAction SilentlyContinue
    }
    
    # Clean other projects that might be linked
    Write-Host "Cleaning Util, Model, and other projects..." -ForegroundColor Gray
    @("Util", "Model", "MainCharacter", "ColorTools", "ObjFacTools", "VideoServices") | ForEach-Object {
        $projPath = "$NetTargetDir\$_\Release"
        if (Test-Path $projPath) {
            Remove-Item -Path $projPath -Recurse -Force -ErrorAction SilentlyContinue
        }
    }
    
    Write-Host "Cleanup complete!" -ForegroundColor Green
}
else {
    Write-Host "Skipping cleanup phase" -ForegroundColor Yellow
}

# Ensure Release directory exists
if (-not (Test-Path $ReleaseDir)) {
    New-Item -ItemType Directory -Path $ReleaseDir -Force | Out-Null
    Write-Host "Created Release directory" -ForegroundColor Gray
}

Write-Host ""
Write-Host "===== BUILD PHASE =====" -ForegroundColor Yellow

# Build VideoServices
Write-Host ""
Write-Host "Building VideoServices.dll..." -ForegroundColor Cyan
$VideoServicesProj = "$NetTargetDir\VideoServices\VideoServices.vcxproj"
& $MSBuildExe $VideoServicesProj /p:Configuration=Release /p:Platform=Win32 /p:OutDir="$ReleaseDir\" /v:quiet 2>&1 | Select-String "error|Error" -ErrorAction SilentlyContinue

if ((Test-Path "$ReleaseDir\VideoServices.dll") -eq $false) {
    Write-Host "ERROR: VideoServices.dll was not created!" -ForegroundColor Red
    exit 1
}

$dll = Get-Item "$ReleaseDir\VideoServices.dll"
Write-Host "SUCCESS: VideoServices.dll created - $($dll.Length) bytes" -ForegroundColor Green

# Copy SDL2.dll
Write-Host ""
Write-Host "Copying SDL2.dll..." -ForegroundColor Cyan
if (Test-Path $SDL2Dll) {
    Copy-Item $SDL2Dll "$ReleaseDir\SDL2.dll" -Force
    Write-Host "SUCCESS: SDL2.dll copied" -ForegroundColor Green
}
else {
    Write-Host "WARNING: SDL2.dll not found at $SDL2Dll" -ForegroundColor Yellow
}

# Build Game2
Write-Host ""
Write-Host "Building Game2.exe..." -ForegroundColor Cyan
$Game2Proj = "$NetTargetDir\Game2\Game2.vcxproj"
& $MSBuildExe $Game2Proj /p:Configuration=Release /p:Platform=Win32 /p:OutDir="$ReleaseDir\" /v:quiet 2>&1 | Select-String "error|Error" -ErrorAction SilentlyContinue

if ((Test-Path "$ReleaseDir\Game2.exe") -eq $false) {
    Write-Host "ERROR: Game2.exe was not created!" -ForegroundColor Red
    exit 1
}

$exe = Get-Item "$ReleaseDir\Game2.exe"
Write-Host "SUCCESS: Game2.exe created - $($exe.Length) bytes" -ForegroundColor Green

Write-Host ""
Write-Host "===== BUILD SUCCESSFUL =====" -ForegroundColor Green
Write-Host "All binaries ready in: $ReleaseDir" -ForegroundColor Green

# Run game if requested
if ($RunGame) {
    Write-Host ""
    Write-Host "===== LAUNCHING GAME =====" -ForegroundColor Yellow
    Write-Host "Starting Game2.exe..." -ForegroundColor Cyan
    Start-Sleep -Milliseconds 500
    
    Push-Location $ReleaseDir
    & ".\Game2.exe" 2>&1
    Pop-Location
}

Write-Host ""
Write-Host "Build process complete!" -ForegroundColor Green
