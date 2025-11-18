@echo off
setlocal

:check
if exist "NetTarget\Game2\Release\Game2.exe" (
    echo Game2.exe found - copying to Release folder...
    copy "NetTarget\Game2\Release\Game2.exe" "Release\Game2.exe" /Y
    if exist "Release\Game2.exe" (
        echo SUCCESS: Game2.exe is now in Release folder
        dir "Release\Game2.exe"
        goto end
    ) else (
        echo ERROR: Failed to copy Game2.exe
        goto end
    )
) else (
    echo Waiting for Game2.exe to be created...
    timeout /t 10 /nobreak
    goto check
)

:end
pause
