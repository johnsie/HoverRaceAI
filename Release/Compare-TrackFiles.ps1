# Compare old vs new track files in detail
Write-Host "=== DETAILED TRACK FILE COMPARISON ===" -ForegroundColor Cyan
Write-Host ""

$newFile = 'C:\originalhr\HoverRace\Release\Tracks\ClassicH.trk'
$oldFile = 'C:\originalhr\HoverRace\Release\Tracks\ClassicH_OLD_2020.trk'

$newBytes = [System.IO.File]::ReadAllBytes($newFile)
$oldBytes = [System.IO.File]::ReadAllBytes($oldFile)

Write-Host "NEW FILE (524,873 bytes):"
Write-Host "  Hex dump (first 300 bytes):"
for ($i = 0; $i -lt [Math]::Min(300, $newBytes.Length); $i += 16) {
    $hexline = ""
    $ascline = ""
    for ($j = 0; $j -lt 16 -and ($i + $j) -lt $newBytes.Length; $j++) {
        $byte = $newBytes[$i + $j]
        $hexline += "$($byte.ToString('X2')) "
        $ascline += if ([char]$byte -match "[\x20-\x7E]") { [char]$byte } else { "." }
    }
    Write-Host "  0x$($i.ToString('X04')): $hexline  | $ascline"
}

Write-Host ""
Write-Host "OLD FILE (572,255 bytes):"
Write-Host "  Hex dump (first 300 bytes):"
for ($i = 0; $i -lt [Math]::Min(300, $oldBytes.Length); $i += 16) {
    $hexline = ""
    $ascline = ""
    for ($j = 0; $j -lt 16 -and ($i + $j) -lt $oldBytes.Length; $j++) {
        $byte = $oldBytes[$i + $j]
        $hexline += "$($byte.ToString('X2')) "
        $ascline += if ([char]$byte -match "[\x20-\x7E]") { [char]$byte } else { "." }
    }
    Write-Host "  0x$($i.ToString('X04')): $hexline  | $ascline"
}

Write-Host ""
Write-Host "RECORD MARKER ANALYSIS:"
$magicNum = 82617  # 0x142B9

# Find all occurrences in both files
Write-Host ""
Write-Host "NEW FILE - Magic numbers at offsets:"
$newMarkers = @()
for ($i = 0; $i -lt $newBytes.Length - 4; $i++) {
    $val = [BitConverter]::ToInt32($newBytes[$i..($i+3)], 0)
    if ($val -eq $magicNum) {
        $newMarkers += $i
        Write-Host "  Offset 0x$($i.ToString('X8')) (Record header expected here)"
    }
}

Write-Host ""
Write-Host "OLD FILE - Magic numbers at offsets:"
$oldMarkers = @()
for ($i = 0; $i -lt $oldBytes.Length - 4; $i++) {
    $val = [BitConverter]::ToInt32($oldBytes[$i..($i+3)], 0)
    if ($val -eq $magicNum) {
        $oldMarkers += $i
        Write-Host "  Offset 0x$($i.ToString('X8')) (Record header expected here)"
    }
}

Write-Host ""
Write-Host "RECORD COUNT SUMMARY:"
Write-Host "  New file has $($newMarkers.Count) record markers"
Write-Host "  Old file has $($oldMarkers.Count) record markers"

if ($newMarkers.Count -gt 1) {
    Write-Host ""
    Write-Host "Record spacing (bytes between markers):"
    for ($i = 1; $i -lt $newMarkers.Count; $i++) {
        $gap = $newMarkers[$i] - $newMarkers[$i-1]
        $prev = $i - 1
        Write-Host "  Records $prev to $i : $gap bytes apart"
    }
}
