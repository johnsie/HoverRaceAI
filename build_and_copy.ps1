#!/usr/bin/env pwsh
# Build and copy DLLs to Release folder

Write-Host "Building MainCharacter.vcxproj..."
& msbuild NetTarget/MainCharacter/MainCharacter.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32 /v:minimal

# Check possible output locations
$possiblePaths = @(
    "C:\originalhr2\HoverRaceAI\Release\MainCharacter.dll",
    "C:\originalhr2\HoverRaceAI\NetTarget\MainCharacter\Release\MainCharacter.dll",
    "C:\originalhr2\HoverRaceAI\NetTarget\MainCharacter\..\..Release\MainCharacter.dll"
)

Write-Host "Checking for MainCharacter.dll in possible locations..."
foreach ($path in $possiblePaths) {
    if (Test-Path $path) {
        Write-Host "Found at: $path"
    }
}

Write-Host "Build completed."
