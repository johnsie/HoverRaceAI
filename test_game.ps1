# Simple test script to run the game and capture output
$ErrorActionPreference = 'SilentlyContinue'

$startTime = Get-Date
Write-Host "Starting Game2.exe at $startTime"

# Start the process and wait max 30 seconds
$process = Start-Process -FilePath "C:\originalhr\HoverRace\Release\Game2.exe" -PassThru -WindowStyle Hidden

# Wait for process to finish or timeout
$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
while ($stopwatch.Elapsed.TotalSeconds -lt 30 -and !$process.HasExited) {
    Start-Sleep -Milliseconds 100
}

$endTime = Get-Date
$elapsedSeconds = $stopwatch.Elapsed.TotalSeconds

Write-Host "Game2.exe ended after $elapsedSeconds seconds"
Write-Host "Process exit code: $($process.ExitCode)"
Write-Host "End time: $endTime"

# List any new logs created
Write-Host "`nLog files created:"
ls C:\originalhr\HoverRace\Release\*.log -ErrorAction SilentlyContinue | ForEach-Object { 
    Write-Host "  $($_.Name) - $($_.Length) bytes"
}

# Terminate if still running
if (!$process.HasExited) {
    Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue
    Write-Host "Process was still running, terminated forcefully"
}
