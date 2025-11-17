@echo off
REM Copy DLLs from Debug to Release directory

echo Copying DLLs from Debug to Release...
copy "C:\originalhr2\HoverRaceAI\Debug\MainCharacter.dll" "C:\originalhr2\HoverRaceAI\Release\MainCharacter.dll" /Y
copy "C:\originalhr2\HoverRaceAI\Debug\Model.dll" "C:\originalhr2\HoverRaceAI\Release\Model.dll" /Y
copy "C:\originalhr2\HoverRaceAI\Debug\VideoServices.dll" "C:\originalhr2\HoverRaceAI\Release\VideoServices.dll" /Y
copy "C:\originalhr2\HoverRaceAI\Debug\Util.dll" "C:\originalhr2\HoverRaceAI\Release\Util.dll" /Y
copy "C:\originalhr2\HoverRaceAI\Debug\ColorTools.dll" "C:\originalhr2\HoverRaceAI\Release\ColorTools.dll" /Y
copy "C:\originalhr2\HoverRaceAI\Debug\ObjFacTools.dll" "C:\originalhr2\HoverRaceAI\Release\ObjFacTools.dll" /Y

echo Done!
dir C:\originalhr2\HoverRaceAI\Release\*.dll
