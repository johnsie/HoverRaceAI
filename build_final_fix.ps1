#!/usr/bin/env pwsh
# Final build script for ObjFac1 and Game2 with fallback rendering fix

$msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
$workdir = "C:\originalhr2\HoverRaceAI"
$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

Write-Host "[$timestamp] Starting final build with fallback rendering fix..." -ForegroundColor Green

# Step 1: Rebuild ObjFac1
Write-Host ""
Write-Host "[$timestamp] STEP 1: Rebuilding ObjFac1.dll with fallback rendering..." -ForegroundColor Yellow
$objfac1Start = Get-Date
& $msbuild "$workdir\NetTarget\ObjFac1\ObjFac1.vcxproj" /t:Rebuild /p:Configuration=Release /p:Platform=Win32 /v:minimal 2>&1 | Out-File -FilePath "$workdir\objfac1_final_build.log"

$objfac1End = Get-Date
$objfac1Duration = [math]::Round(($objfac1End - $objfac1Start).TotalSeconds, 1)
Write-Host "ObjFac1 build completed in $objfac1Duration seconds" -ForegroundColor Cyan

# Check for errors
$errors = Select-String -Path "$workdir\objfac1_final_build.log" -Pattern "error" -CaseSensitive
if ($errors) {
    Write-Host "ERRORS FOUND IN OBJFAC1 BUILD:" -ForegroundColor Red
    $errors | Select-Object -First 10
    exit 1
}

Write-Host "ObjFac1 build successful!" -ForegroundColor Green

# Step 2: Rebuild Game2
Write-Host ""
Write-Host "[$timestamp] STEP 2: Rebuilding Game2.exe with new ObjFac1..." -ForegroundColor Yellow
$game2Start = Get-Date
& $msbuild "$workdir\NetTarget\Game2\Game2.vcxproj" /t:Rebuild /p:Configuration=Release /p:Platform=Win32 /v:minimal 2>&1 | Out-File -FilePath "$workdir\game2_final_build.log"

$game2End = Get-Date
$game2Duration = [math]::Round(($game2End - $game2Start).TotalSeconds, 1)
Write-Host "Game2 build completed in $game2Duration seconds" -ForegroundColor Cyan

# Check for errors
$errors = Select-String -Path "$workdir\game2_final_build.log" -Pattern "error" -CaseSensitive
if ($errors) {
    Write-Host "ERRORS FOUND IN GAME2 BUILD:" -ForegroundColor Red
    $errors | Select-Object -First 10
    exit 1
}

Write-Host "Game2 build successful!" -ForegroundColor Green

# Step 3: Verify binaries exist
Write-Host ""
Write-Host "[$timestamp] STEP 3: Verifying binaries..." -ForegroundColor Yellow
$game2exe = Get-Item "$workdir\Release\Game2.exe" -ErrorAction SilentlyContinue
if ($game2exe) {
    Write-Host "✓ Game2.exe: $($game2exe.FullName) ($($game2exe.Length) bytes, $(Get-Date -Date $game2exe.LastWriteTime -Format 'HH:mm:ss'))" -ForegroundColor Green
} else {
    Write-Host "✗ Game2.exe NOT FOUND" -ForegroundColor Red
    exit 1
}

# Step 4: Summary
Write-Host ""
Write-Host "[$timestamp] BUILD COMPLETE!" -ForegroundColor Green
Write-Host "Total build time: $([math]::Round((($game2End - $objfac1Start).TotalSeconds), 1)) seconds"
Write-Host ""
Write-Host "The fallback rendering fix is now deployed:" -ForegroundColor Cyan
Write-Host "  - 10 million unit frustum tolerance (effectively disables frustum culling)"
Write-Host "  - Identity matrix fallback rendering when matrix computation fails"
Write-Host "  - Hovercraft will ALWAYS be visible, never disappear"
Write-Host ""
Write-Host "Ready to test in-game!" -ForegroundColor Green
