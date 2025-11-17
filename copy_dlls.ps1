# Copy DLLs from Debug to Release
Write-Host "Copying DLLs..."

$files = @(
    'C:\originalhr2\HoverRaceAI\Debug\MainCharacter.dll',
    'C:\originalhr2\HoverRaceAI\Debug\MainCharacter.lib',
    'C:\originalhr2\HoverRaceAI\Debug\MainCharacter.exp',
    'C:\originalhr2\HoverRaceAI\Debug\Model.dll',
    'C:\originalhr2\HoverRaceAI\Debug\Model.lib',
    'C:\originalhr2\HoverRaceAI\Debug\Model.exp',
    'C:\originalhr2\HoverRaceAI\Debug\VideoServices.dll',
    'C:\originalhr2\HoverRaceAI\Debug\VideoServices.lib',
    'C:\originalhr2\HoverRaceAI\Debug\VideoServices.exp'
)

foreach ($file in $files) {
    if (Test-Path $file) {
        Copy-Item -Path $file -Destination 'C:\originalhr2\HoverRaceAI\Release\' -Force
        Write-Host "Copied: $(Split-Path $file -Leaf)"
    }
}

Write-Host "Done. Verifying:"
Get-ChildItem 'C:\originalhr2\HoverRaceAI\Release\MainCharacter.dll' -ErrorAction SilentlyContinue | Write-Host
Get-ChildItem 'C:\originalhr2\HoverRaceAI\Release\Model.dll' -ErrorAction SilentlyContinue | Write-Host
