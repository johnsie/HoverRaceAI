# Script to examine .trk file structure
param(
    [string]$TrkFile = "Tracks\ClassicH.trk"
)

if (-not (Test-Path $TrkFile)) {
    Write-Error "Track file not found: $TrkFile"
    exit 1
}

$file = Get-Item $TrkFile
Write-Host "Track File: $($file.Name)"
Write-Host "Size: $($file.Length) bytes"
Write-Host "Modified: $($file.LastWriteTime)"
Write-Host ""

# Read the header
$bytes = [System.IO.File]::ReadAllBytes($file.FullName)

# Check copyright notice (first ~60 bytes)
$headerBytes = $bytes[0..100]
$headerText = [System.Text.Encoding]::ASCII.GetString($headerBytes)
Write-Host "Header (ASCII):"
Write-Host $headerText.Substring(0, [Math]::Min(60, $headerText.Length))
Write-Host ""

# Try to find Record 0 magic number (82617 = 0x142B9)
$magicBytes = [BitConverter]::GetBytes([int32]82617)
Write-Host "Looking for Record 0 magic number (82617): "
for ($i = 0; $i -lt $bytes.Length - 4; $i++) {
    $testBytes = $bytes[$i..($i+3)]
    $value = [BitConverter]::ToInt32($testBytes, 0)
    if ($value -eq 82617) {
        Write-Host "  Found at offset 0x$($i.ToString('X'))"
    }
}

Write-Host ""
Write-Host "File hex dump (first 200 bytes):"
$hexDump = ""
for ($i = 0; $i -lt [Math]::Min(200, $bytes.Length); $i++) {
    if ($i -mod 16 -eq 0) {
        $hexDump += "`n0x$($i.ToString('X4')): "
    }
    $hexDump += "$($bytes[$i].ToString('X2')) "
}
Write-Host $hexDump
