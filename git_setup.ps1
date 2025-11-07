# Initialize git and commit the clean project files
Set-Location c:\originalhr\HoverRace

# Set git user config if not already set
git config --global user.name "HoverRace Build" 2>$null
git config --global user.email "build@hoverrace.local" 2>$null

# Check if repo is initialized
if (-not (Test-Path ".git")) {
    Write-Host "Initializing git repository..."
    git init
    git config user.name "HoverRace Build"
    git config user.email "build@hoverrace.local"
}

# Add the clean project files
Write-Host "Adding clean project files..."
git add DEPLOY_Util.vcxproj
git add DEPLOY_VideoServices.vcxproj
git add DEPLOY_Model.vcxproj
git add DEPLOY_MainCharacter.vcxproj
git add DEPLOY_ObjFacTools.vcxproj
git add DEPLOY_ColorTools.vcxproj
git add deploy.cmd

# Check status
Write-Host "`nGit status:"
git status

# Make initial commit
Write-Host "`nCreating initial commit..."
git commit -m "Initial commit: Add clean .vcxproj files and deployment script

- Created 6 clean .vcxproj files from GitHub originals (commit 0f06d7a)
- Files: Util, VideoServices, Model, MainCharacter, ObjFacTools, ColorTools
- Added DEPLOY.cmd script to copy files to correct locations
- Fixes corrupted project files due to previous build system issues"

Write-Host "`nCommit complete!"
git log --oneline -5
