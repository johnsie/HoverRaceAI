
# Comprehensive track file verification script
param(
    [string]$TrkFile = "Tracks\ClassicH.trk",
    [string]$OldTrkFile = "Tracks\ClassicH_OLD_2020.trk"
)

Write-Host "=== TRACK FILE COMPARISON AND VERIFICATION ===" -ForegroundColor Cyan
Write-Host ""

# Check both files
$newFile = Get-Item $TrkFile -ErrorAction SilentlyContinue
$oldFile = Get-Item $OldTrkFile -ErrorAction SilentlyContinue

if (-not $newFile) {
    Write-Host "ERROR: New track file not found: $TrkFile" -ForegroundColor Red
    exit 1
}

Write-Host "NEW TRACK FILE (Just Compiled with Fixed MazeCompiler):"
Write-Host "  Path: $($ newFile.FullName)"
Write-Host "  Size: $($newFile.Length) bytes"
Write-Host "  Modified: $($newFile.LastWriteTime)"
Write-Host ""

if ($oldFile) {
    Write-Host "OLD TRACK FILE (Compiled 2020, BEFORE fix):"
    Write-Host "  Path: $($oldFile.FullName)"
    Write-Host "  Size: $($oldFile.Length) bytes"
    Write-Host "  Modified: $($oldFile.LastWriteTime)"
    Write-Host ""
    
    $sizeDiff = $newFile.Length - $oldFile.Length
    Write-Host "SIZE DIFFERENCE: $sizeDiff bytes"
    if ($sizeDiff -gt 0) {
        Write-Host "  New file is LARGER (contains more data - likely has Record 1)" -ForegroundColor Green
    } else {
        Write-Host "  New file is SMALLER" -ForegroundColor Yellow
    }
    Write-Host ""
}

# Read binary structure
$bytes = [System.IO.File]::ReadAllBytes($newFile.FullName)

# Check copyright notice
$headerBytes = $bytes[0..80]
$headerText = [System.Text.Encoding]::ASCII.GetString($headerBytes)
Write-Host "FILE HEADER (ASCII):"
$headerLine = $headerText.Split("`r`n")[0]
Write-Host "  '$headerLine'"
Write-Host ""

# Look for Record structure markers (magic numbers)
$magicNumber = 82617  # 0x142B9 in decimal
$magicBytes = [BitConverter]::GetBytes([int32]$magicNumber)

Write-Host "SEARCHING FOR RECORD MARKERS (Magic Number: $magicNumber = 0x$([Convert]::ToString($magicNumber, 16))):"
$recordCount = 0
for ($i = 0; $i -lt $bytes.Length - 4; $i++) {
    $testBytes = $bytes[$i..($i+3)]
    $value = [BitConverter]::ToInt32($testBytes, 0)
    if ($value -eq $magicNumber) {
        $recordCount++
        Write-Host "  Record $recordCount found at offset 0x$($i.ToString('X')) (byte $i)"
    }
}

Write-Host ""
Write-Host "TOTAL RECORD MARKERS FOUND: $recordCount"
if ($recordCount -ge 3) {
    Write-Host "  ✓ SUCCESS: File has multiple record markers (expected: Records 0-4)" -ForegroundColor Green
} elseif ($recordCount -eq 1) {
    Write-Host "  ⚠ WARNING: File only has 1 record marker (may be missing level geometry)" -ForegroundColor Yellow
} else {
    Write-Host "  ✗ ERROR: File has no or invalid record markers" -ForegroundColor Red
}

Write-Host ""
Write-Host "FILE SIZE ANALYSIS:"
Write-Host "  Total size: $($bytes.Length) bytes"
Write-Host "  Expected structure:"
Write-Host "    - Header (~100 bytes)"
Write-Host "    - Record 0 (Header metadata ~200-500 bytes)"
Write-Host "    - Record 1 (Level geometry - SHOULD BE LARGE if present)"
Write-Host "    - Records 2-4 (Background, map, MIDI)"
Write-Host ""

if ($bytes.Length -gt 300000) {
    Write-Host "  ✓ File size suggests proper Record 1 with geometry data" -ForegroundColor Green
} elseif ($bytes.Length -gt 200000) {
    Write-Host "  ⚠ File size is moderate - may have limited geometry" -ForegroundColor Yellow
} else {
    Write-Host "  ✗ File size is too small - likely missing Record 1" -ForegroundColor Red
}

Write-Host ""
Write-Host "=== VERIFICATION SUMMARY ===" -ForegroundColor Cyan
if ($recordCount -ge 2 -and $bytes.Length -gt 200000) {
    Write-Host "✓ Track file appears to be properly compiled!" -ForegroundColor Green
    Write-Host "  - Multiple record markers present"
    Write-Host "  - Reasonable file size for complete track"
    Write-Host "  - Ready to test with Game2.exe"
} else {
    Write-Host "✗ Track file may have issues:" -ForegroundColor Red
    if ($recordCount -lt 2) {
        Write-Host "  - Only $recordCount record marker(s) found (expected at least 4)"
    }
    if ($bytes.Length -le 200000) {
        Write-Host "  - File size $($bytes.Length) bytes seems too small"
    }
}
