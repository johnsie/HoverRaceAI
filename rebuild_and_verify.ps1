#!/usr/bin/env pwsh
# HoverRaceAI Build and Verify Script
# Ensures binaries are built to correct location and verifies the output

$projects = @(
    "C:\originalhr2\HoverRaceAI\NetTarget\VideoServices\VideoServices.vcxproj",
    "C:\originalhr2\HoverRaceAI\NetTarget\Game2\Game2.vcxproj"
)

$outputDir = "C:\originalhr2\HoverRaceAI\Release\"
$msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

# Track build times
$previewTimes = @{}
$projects | ForEach-Object {
    $proj = $_
    $name = (Split-Path $proj -Leaf) -replace '\.vcxproj', ''
    Write-Host "BEFORE: Checking $name"
    Get-ChildItem "$outputDir$name.dll" -ErrorAction SilentlyContinue | ForEach-Object {
        $previewTimes[$name] = $_.LastWriteTime
        Write-Host "  $name.dll modified: $($_.LastWriteTime)"
    }
    Get-ChildItem "$outputDir$name.exe" -ErrorAction SilentlyContinue | ForEach-Object {
        $previewTimes[$name] = $_.LastWriteTime
        Write-Host "  $name.exe modified: $($_.LastWriteTime)"
    }
}

Write-Host "`n=== BUILDING ===" 

# Build each project
$projects | ForEach-Object {
    $proj = $_
    Write-Host "`nBuilding: $(Split-Path $proj -Parent | Split-Path -Leaf)"
    & $msbuild $proj /p:Configuration=Release /p:Platform=Win32 /p:OutDir=$outputDir /m 2>&1 | tail -5
}

Write-Host "`n=== VERIFICATION ===" 

# Verify outputs were updated
$projects | ForEach-Object {
    $proj = $_
    $name = (Split-Path $proj -Leaf) -replace '\.vcxproj', ''
    Write-Host "`nAFTER: Checking $name"
    
    $found = $false
    $outputFile = Get-ChildItem "$outputDir$name.exe" -ErrorAction SilentlyContinue
    if (!$outputFile) {
        $outputFile = Get-ChildItem "$outputDir$name.dll" -ErrorAction SilentlyContinue
    }
    
    if ($outputFile) {
        $found = $true
        $oldTime = $previewTimes[$name]
        $newTime = $outputFile.LastWriteTime
        
        if ($oldTime -and $newTime -gt $oldTime) {
            Write-Host "✓ $($outputFile.Name) was UPDATED" -ForegroundColor Green
        } elseif ($oldTime) {
            Write-Host "⚠ $($outputFile.Name) was NOT updated (still old: $newTime)" -ForegroundColor Yellow
        } else {
            Write-Host "✓ $($outputFile.Name) created: $newTime" -ForegroundColor Green
        }
        Write-Host "  Size: $(($outputFile.Length / 1MB).ToString('N2')) MB"
    } else {
        Write-Host "✗ $name output NOT FOUND in $outputDir" -ForegroundColor Red
    }
}

Write-Host "`nGame2.exe final check:"
$game2 = Get-Item "C:\originalhr2\HoverRaceAI\Release\Game2.exe" -ErrorAction SilentlyContinue
if ($game2) {
    Write-Host "✓ Game2.exe ready to run" -ForegroundColor Green
    Write-Host "  Path: $($game2.FullName)"
    Write-Host "  Size: $(($game2.Length / 1MB).ToString('N2')) MB"
    Write-Host "  Modified: $($game2.LastWriteTime)"
} else {
    Write-Host "✗ CRITICAL: Game2.exe not found!" -ForegroundColor Red
}
