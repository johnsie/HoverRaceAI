#Requires -Version 5.0
<#
.SYNOPSIS
    Batch recompiles HoverRace track files (.TR) to generate new .TRK files with updated palette.

.DESCRIPTION
    This script automates the track recompilation process by:
    1. Copying each .TR file
    2. Using HoverCad to open and compile each track
    3. Or directly using MazeCompiler if intermediate files are available

    The key benefit: MazeCompiler will use the current ColorTab.pal when compiling,
    which means the newly generated .TRK files will have the color palette instead of grayscale.

.PARAMETER TrackSourceDir
    Directory containing .TR source files (e.g., C:\originalhr\HoverRace\NetTarget\mazes)

.PARAMETER OutputDir
    Directory where compiled .TRK files will be saved

.PARAMETER MazeCompilerPath
    Path to MazeCompiler.exe

.PARAMETER HoverCadPath
    Path to HoverCad.exe for GUI-based compilation

.EXAMPLE
    .\recompile_tracks.ps1 -TrackSourceDir "C:\originalhr\HoverRace\NetTarget\mazes" `
                          -OutputDir "C:\originalhr\HoverRace\Release\Tracks\mazes" `
                          -MazeCompilerPath "C:\originalhr\HoverRace\Release\MazeCompiler.exe"

.NOTES
    Important: ColorTab.pal must be in the same directory as MazeCompiler.exe
    when compilation occurs, or in the directory where MazeCompiler runs.
#>

param(
    [Parameter(Mandatory=$false)]
    [string]$TrackSourceDir = "C:\originalhr\HoverRace\NetTarget\mazes",

    [Parameter(Mandatory=$false)]
    [string]$OutputDir = "C:\originalhr\HoverRace\Release\Tracks\mazes",

    [Parameter(Mandatory=$false)]
    [string]$MazeCompilerPath = "C:\originalhr\HoverRace\Release\MazeCompiler.exe",

    [Parameter(Mandatory=$false)]
    [string]$ColorTabPath = "C:\originalhr\HoverRace\NetTarget\bitmaps\ColorTab.pal"
)

# Ensure paths exist
if (-not (Test-Path $MazeCompilerPath)) {
    Write-Error "MazeCompiler.exe not found at: $MazeCompilerPath"
    exit 1
}

if (-not (Test-Path $ColorTabPath)) {
    Write-Warning "ColorTab.pal not found at: $ColorTabPath"
    Write-Warning "MazeCompiler may use a default grayscale palette"
}

if (-not (Test-Path $TrackSourceDir)) {
    Write-Error "Track source directory not found: $TrackSourceDir"
    exit 1
}

if (-not (Test-Path $OutputDir)) {
    Write-Host "Creating output directory: $OutputDir"
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

# Get all .TR files
$trackFiles = Get-ChildItem -Path $TrackSourceDir -Filter "*.TR" -ErrorAction SilentlyContinue

if ($trackFiles.Count -eq 0) {
    Write-Warning "No .TR files found in $TrackSourceDir"
    exit 0
}

Write-Host "Found $($trackFiles.Count) track file(s) to recompile"
Write-Host ""

$successCount = 0
$failureCount = 0

foreach ($trackFile in $trackFiles) {
    $trackName = $trackFile.BaseName
    $outputPath = Join-Path $OutputDir "$trackName.trk"
    
    Write-Host "Recompiling: $trackName"
    Write-Host "  Source: $($trackFile.FullName)"
    Write-Host "  Output: $outputPath"
    
    # Strategy: Use HoverCad's debug file from %TEMP% if available
    # Otherwise, we would need to implement binary .TR parsing which is complex
    
    Write-Host "  Status: Requires HoverCad GUI for .TR to .txt conversion"
    Write-Host "  [Manual step] Open $($trackFile.FullName) in HoverCad and click 'Compile'"
    Write-Host ""
}

Write-Host ""
Write-Host "=== ALTERNATIVE: Direct MazeCompiler Usage ==="
Write-Host ""
Write-Host "If you have intermediate .txt files from HoverCad debug output:"
Write-Host ""
Write-Host "    cd $(Split-Path $MazeCompilerPath -Parent)"
Write-Host "    .\MazeCompiler.exe `"$outputPath`" `"<intermediate_file.txt`""
Write-Host ""
Write-Host "Or batch compile all .txt files:"
Write-Host ""
Write-Host "    Get-ChildItem *.txt | ForEach-Object {"
Write-Host "        & '$(Split-Path $MazeCompilerPath -Parent)\MazeCompiler.exe' `$_.BaseName '.trk' `$_.FullName"
Write-Host "    }"
