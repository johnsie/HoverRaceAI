#!/usr/bin/pwsh
# Script to compile and test the fixed track

cd 'c:\originalhr\HoverRace\Release'

$logFile = "compile_test.log"

"=== Compiling ClassicH track with fixed MazeCompiler ===" | Tee-Object $logFile
"Time: $(Get-Date)" | Tee-Object $logFile -Append

$debugFile = "$env:TEMP\HCA972.tmp.debug"
$outputFile = "Tracks\ClassicH_fixed_v2.trk"

"Input: $debugFile" | Tee-Object $logFile -Append
"Output: $outputFile" | Tee-Object $logFile -Append
"" | Tee-Object $logFile -Append

# Run MazeCompiler
"Running: .\MazeCompiler.exe '$outputFile' '$debugFile'" | Tee-Object $logFile -Append
Write-Host "Executing MazeCompiler..." -ForegroundColor Green

$proc = Start-Process -FilePath ".\MazeCompiler.exe" -ArgumentList "'$outputFile'", "'$debugFile'" -NoNewWindow -PassThru -RedirectStandardOutput "mazecompiler_output.txt" -RedirectStandardError "mazecompiler_error.txt"

$exitCode = $proc.ExitCode
Write-Host "Process started with ID: $($proc.Id)"
Start-Sleep -Seconds 5
$proc | Wait-Process
$exitCode = $proc.ExitCode

"" | Tee-Object $logFile -Append
"Exit code: $exitCode" | Tee-Object $logFile -Append

if (Test-Path "mazecompiler_output.txt") {
    "--- MazeCompiler Output ---" | Tee-Object $logFile -Append
    Get-Content "mazecompiler_output.txt" | Tee-Object $logFile -Append
}

if (Test-Path "mazecompiler_error.txt") {
    "--- MazeCompiler Errors ---" | Tee-Object $logFile -Append
    Get-Content "mazecompiler_error.txt" | Tee-Object $logFile -Append
}

# Check output file
"" | Tee-Object $logFile -Append
if (Test-Path $outputFile) {
    $fileInfo = Get-Item $outputFile
    "✓ Output file created: $($fileInfo.Length) bytes" | Tee-Object $logFile -Append
    "File timestamp: $($fileInfo.LastWriteTime)" | Tee-Object $logFile -Append
} else {
    "✗ Output file NOT created" | Tee-Object $logFile -Append
}

"" | Tee-Object $logFile -Append
"=== Compilation Complete ===" | Tee-Object $logFile -Append
"Log saved to: $logFile" | Tee-Object $logFile -Append

# Print to console too
Write-Host "Compilation complete. Exit code: $exitCode"
Get-Content $logFile | Write-Host
