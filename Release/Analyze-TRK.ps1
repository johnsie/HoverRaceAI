# TRK File Analyzer in PowerShell
# Analyzes the binary structure of ClassicH.trk

$filename = "Release\Tracks\ClassicH.trk"
$logFile = "Release\trk_analysis.log"

"=== TRK File Analyzer ===" | Tee-Object $logFile -Append
"File: $(Get-Item $filename | Select-Object FullName, Length)" | Tee-Object $logFile -Append

# Read file bytes
$bytes = [System.IO.File]::ReadAllBytes($filename)
$fileSize = $bytes.Length

Write-Host "`n=== File Analysis ===" -ForegroundColor Green
"File size: $fileSize bytes (0x{0:X})" -f $fileSize | Tee-Object $logFile -Append

# Read copyright header
$encoding = [System.Text.Encoding]::ASCII
$firstLine = $encoding.GetString($bytes, 0, 50)
"First bytes (ASCII): $firstLine" | Tee-Object $logFile -Append

# Magic number to search for: 82617 = 0x1427F
# Little-endian: 7F 14 00 00
$magicNumber = 82617  # MR_MAGIC_TRACK_NUMBER
"Looking for magic number: $magicNumber (0x{0:X})`n" -f $magicNumber | Tee-Object $logFile -Append

# Convert magic number to little-endian bytes
$magic_bytes = [System.BitConverter]::GetBytes([Int32]$magicNumber)
"Magic number little-endian bytes: {0:X2} {1:X2} {2:X2} {3:X2}" -f $magic_bytes[0], $magic_bytes[1], $magic_bytes[2], $magic_bytes[3] | Tee-Object $logFile -Append

# Search for magic number in file
$magicCount = 0
$magicPositions = @()

for ($i = 0; $i -lt $bytes.Length - 3; $i++) {
    $value = [System.BitConverter]::ToInt32($bytes, $i)
    if ($value -eq $magicNumber) {
        $magicPositions += $i
        $magicCount++
    }
}

"`nMagic number found $magicCount times at offsets:" | Tee-Object $logFile -Append
foreach ($pos in $magicPositions) {
    "  0x{0:X} ({0})" -f $pos | Tee-Object $logFile -Append
}

# Hex dump of first 512 bytes
"`n=== HEX DUMP (First 512 bytes) ===" | Tee-Object $logFile -Append
$dumpSize = [Math]::Min(512, $fileSize)
for ($i = 0; $i -lt $dumpSize; $i += 16) {
    $hexPart = ""
    $asciiPart = ""
    
    for ($j = 0; $j -lt 16 -and $i + $j -lt $dumpSize; $j++) {
        $byte = $bytes[$i + $j]
        $hexPart += "{0:X2} " -f $byte
        
        if ($byte -ge 32 -and $byte -lt 127) {
            $asciiPart += [char]$byte
        } else {
            $asciiPart += "."
        }
    }
    
    "{0:X8}: {1,-48} | {2}" -f $i, $hexPart, $asciiPart | Tee-Object $logFile -Append
}

# Look for readable strings
"`n=== READABLE STRINGS ===" | Tee-Object $logFile -Append
$text = $encoding.GetString($bytes)
$strings = $text -split "[\x00-\x1F\x7F]+" | Where-Object {$_.Length -gt 4} | Select-Object -Unique

"Found {0} unique readable strings:" -f $strings.Count | Tee-Object $logFile -Append
$strings | ForEach-Object {
    if ($_.Length -lt 100) {
        "`"$_`"" | Tee-Object $logFile -Append
    }
}

# Try to identify record boundaries
"`n=== RECORD STRUCTURE ANALYSIS ===" | Tee-Object $logFile -Append

# Look for patterns that might indicate record offsets
# CArchive in MFC with BeginANewRecord typically writes:
# - Some prefix/metadata
# - Then the record data

# Check for 4-byte aligned offsets or size patterns
"Analyzing 4-byte words for potential record offsets..." | Tee-Object $logFile -Append

$dwords = @()
for ($i = 0; $i -lt [Math]::Min(256, $fileSize - 3); $i += 4) {
    $dword = [System.BitConverter]::ToInt32($bytes, $i)
    if ($dword -gt 0 -and $dword -lt $fileSize) {
        $dwords += @{Offset = $i; Value = $dword}
        "  Offset 0x{0:X}: Potential file offset 0x{1:X} ({1})" -f $i, $dword | Tee-Object $logFile -Append
    }
}

# Look for CString serialized data
"`n=== LOOKING FOR SERIALIZED STRINGS ===" | Tee-Object $logFile -Append
# CString in MFC is serialized as:
# - If length < 0xFFFF: write (UINT16)length, then chars
# - If empty: write 0xFFFF

# Let's search for "Description" or "Level" pattern which would appear after serialization
$searchPatterns = @("Level", "Length", "Easy", "Short and fast", "Lots of curve")
foreach ($pattern in $searchPatterns) {
    $patternBytes = $encoding.GetBytes($pattern)
    for ($i = 0; $i -lt $fileSize - $patternBytes.Length; $i++) {
        $match = $true
        for ($j = 0; $j -lt $patternBytes.Length; $j++) {
            if ($bytes[$i + $j] -ne $patternBytes[$j]) {
                $match = $false
                break
            }
        }
        if ($match) {
            "Found '$pattern' at offset 0x{0:X}" -f $i | Tee-Object $logFile -Append
            # Show context
            $start = [Math]::Max(0, $i - 8)
            $end = [Math]::Min($fileSize - 1, $i + $patternBytes.Length + 8)
            "Context: " | Tee-Object $logFile -Append
            $hex = ""
            for ($k = $start; $k -le $end; $k++) {
                $hex += "{0:X2} " -f $bytes[$k]
            }
            $hex | Tee-Object $logFile -Append
        }
    }
}

"`n=== ANALYSIS COMPLETE ===" | Tee-Object $logFile -Append
"Log saved to: $logFile" | Tee-Object $logFile -Append
