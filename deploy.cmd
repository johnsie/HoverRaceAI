@echo off
REM Deploy clean project files over corrupted ones
cd /d c:\originalhr\HoverRace

echo Deploying clean project files...

REM Delete and replace Util
if exist "DEPLOY_Util.vcxproj" (
    del /q "NetTarget\Util\Util.vcxproj" 2>nul
    copy "DEPLOY_Util.vcxproj" "NetTarget\Util\Util.vcxproj" >nul
    echo [OK] Util.vcxproj
)

REM Delete and replace VideoServices
if exist "DEPLOY_VideoServices.vcxproj" (
    del /q "NetTarget\VideoServices\VideoServices.vcxproj" 2>nul
    copy "DEPLOY_VideoServices.vcxproj" "NetTarget\VideoServices\VideoServices.vcxproj" >nul
    echo [OK] VideoServices.vcxproj
)

REM Delete and replace Model
if exist "DEPLOY_Model.vcxproj" (
    del /q "NetTarget\Model\Model.vcxproj" 2>nul
    copy "DEPLOY_Model.vcxproj" "NetTarget\Model\Model.vcxproj" >nul
    echo [OK] Model.vcxproj
)

REM Delete and replace MainCharacter
if exist "DEPLOY_MainCharacter.vcxproj" (
    del /q "NetTarget\MainCharacter\MainCharacter.vcxproj" 2>nul
    copy "DEPLOY_MainCharacter.vcxproj" "NetTarget\MainCharacter\MainCharacter.vcxproj" >nul
    echo [OK] MainCharacter.vcxproj
)

REM Delete and replace ObjFacTools
if exist "DEPLOY_ObjFacTools.vcxproj" (
    del /q "NetTarget\ObjFacTools\ObjFacTools.vcxproj" 2>nul
    copy "DEPLOY_ObjFacTools.vcxproj" "NetTarget\ObjFacTools\ObjFacTools.vcxproj" >nul
    echo [OK] ObjFacTools.vcxproj
)

REM Delete and replace ColorTools
if exist "DEPLOY_ColorTools.vcxproj" (
    del /q "NetTarget\ColorTools\ColorTools.vcxproj" 2>nul
    copy "DEPLOY_ColorTools.vcxproj" "NetTarget\ColorTools\ColorTools.vcxproj" >nul
    echo [OK] ColorTools.vcxproj
)

echo.
echo All project files deployed successfully!
echo.
pause
