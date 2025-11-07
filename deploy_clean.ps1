# Move clean files over corrupted ones
$moves = @(
    ("Util_clean.vcxproj", "NetTarget\Util\Util.vcxproj"),
    ("VideoServices_clean.vcxproj", "NetTarget\VideoServices\VideoServices.vcxproj"),
    ("Model_clean.vcxproj", "NetTarget\Model\Model.vcxproj"),
    ("MainCharacter_clean.vcxproj", "NetTarget\MainCharacter\MainCharacter.vcxproj"),
    ("ObjFacTools_clean.vcxproj", "NetTarget\ObjFacTools\ObjFacTools.vcxproj"),
    ("ColorTools_clean.vcxproj", "NetTarget\ColorTools\ColorTools.vcxproj")
)

$basePath = "c:\originalhr\HoverRace"

foreach ($move in $moves) {
    $src = Join-Path $basePath $move[0]
    $dst = Join-Path $basePath $move[1]
    
    if (Test-Path $src) {
        # Delete destination if exists
        if (Test-Path $dst) {
            Remove-Item -Path $dst -Force
        }
        # Copy clean version
        Copy-Item -Path $src -Destination $dst -Force
        Write-Host "Replaced $(Split-Path $dst -Leaf)"
    } else {
        Write-Host "Source not found: $src"
    }
}

Write-Host "All project files have been replaced with clean versions."
