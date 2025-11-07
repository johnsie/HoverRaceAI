@echo off
REM Initialize git repo and commit clean project files

cd /d c:\originalhr\HoverRace

echo.
echo Setting up git repository...
echo.

REM Configure git user (global)
git config --global user.name "HoverRace Build" >nul 2>&1
git config --global user.email "build@hoverrace.local" >nul 2>&1

REM Check if .git exists, if not init
if not exist ".git" (
    echo Initializing git repository...
    git init
    git config user.name "HoverRace Build"
    git config user.email "build@hoverrace.local"
    echo.
)

REM Add clean project files
echo Adding clean project files to git...
git add DEPLOY_Util.vcxproj
git add DEPLOY_VideoServices.vcxproj
git add DEPLOY_Model.vcxproj
git add DEPLOY_MainCharacter.vcxproj
git add DEPLOY_ObjFacTools.vcxproj
git add DEPLOY_ColorTools.vcxproj
git add deploy.cmd
git add git_setup.ps1

REM Check status
echo.
echo Current git status:
git status

REM Commit
echo.
echo Creating initial commit...
git commit -m "Initial commit: Clean project files and deployment scripts

- Created 6 clean .vcxproj files from GitHub originals (commit 0f06d7a)
- Projects: Util, VideoServices, Model, MainCharacter, ObjFacTools, ColorTools
- Added DEPLOY.cmd to copy files to NetTarget directories
- Fixed corrupted project files from previous build system issues"

echo.
echo Git setup complete!
echo.
git log --oneline -5
echo.
pause
