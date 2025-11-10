# HoverRace - Quick Reference Guide

## Essential Commands

### Build Game
```powershell
cd c:\originalhr\HoverRace
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\Game2\Game2.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32"
```

### Build & See Summary
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\Game2\Game2.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32" 2>&1 | Select-Object -Last 10
```

### Run Game (Graphical)
```powershell
cd c:\originalhr\HoverRace\Release
.\Game2.exe
```

### Run Game (Background for Logging)
```powershell
cd c:\originalhr\HoverRace\Release
rm -Force Game2_*.log
Start-Process .\Game2.exe -WindowStyle Hidden
Start-Sleep -Seconds 30
```

### Check Logs
```powershell
cd c:\originalhr\HoverRace\Release
type Game2_TrackLoad.log         # Track loading info
type Game2_Actor_Render.log      # Actor rendering (should show actor count)
type Game2_Character_Debug.log   # Character position/room
type Game2_InsertElement.log     # Element insertion status
type Game2_AddRenderer.log       # Renderer initialization
```

### Build Individual Projects
```powershell
# Model DLL
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\Model\Model.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32"

# MainCharacter DLL
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\MainCharacter\MainCharacter.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32"

# Game2 (depends on all DLLs)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\Game2\Game2.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32"
```

## Key Files to Edit

### Graphics & Rendering
- `NetTarget/Game2/Observer.cpp` - Main 3D rendering pipeline
- `NetTarget/VideoServices/SDL2Graphics.cpp` - Graphics backend

### Character System
- `NetTarget/MainCharacter/MainCharacter.cpp` - Player character (✅ AddRenderer fixed)
- `NetTarget/Game2/ClientSession.cpp` - Character creation & initialization

### Game Logic
- `NetTarget/Model/Level.cpp` - Level/room management (✅ InsertElement logging added)
- `NetTarget/Game2/GameApp.cpp` - Main game loop

## Current Status

✅ **HOVERCRAFT IS NOW VISIBLE AND RENDERING**

### What's Working
- Graphics rendering: 6700+ frames/run
- Window display: 960x720 resolution
- Color system: RGB24 palette format
- Character initialization: Position (369037, 65817, 0)
- Actor insertion: Successfully inserted into level
- Actor rendering: Found every frame and rendered

### Known Logs to Check for Success
1. `Game2_InsertElement.log` should show: "InsertElement END: returning ..."
2. `Game2_Actor_Render.log` should show: "Actor 0 in room 0: pos=(...) valid=1"
3. `Game2_AddRenderer.log` should show: "SEH EXCEPTION in CreateObject ... continuing"

## Troubleshooting

### Game Won't Start
- Check if Game2.exe exists: `ls c:\originalhr\HoverRace\Release\Game2.exe`
- Verify all DLLs present: `ls c:\originalhr\HoverRace\Release\*.dll | measure`

### Hovercraft Not Visible
- Check `Game2_Actor_Render.log` for "No actors in room 0"
- Check `Game2_InsertElement.log` for "EXCEPTION in AddRenderer()"
- Verify position valid: `Game2_Character_Debug.log`

### Build Errors
- Ensure all projects build: Start with Model, then MainCharacter, then Game2
- Check for "Error(s)" in MSBuild output (warnings are OK)
- Look for missing DLL errors at runtime

### Performance Issues
- Current: ~110 FPS
- If slower, check: CPU usage, memory leaks, file I/O from logging

## Documentation Files

- `BUILD_INSTRUCTIONS.md` - Detailed build guide
- `HOVERCRAFT_FIX_COMPLETE.md` - Complete fix explanation
- `.copilot-instructions.md` - AI copilot reference

## Next Steps for Development

1. Test on all 4 available tracks
2. Test multiplayer (second character)
3. Test UI (track selection, menus)
4. Optimize logging (remove file I/O in final build)
5. Implement proper error handling for DLL loading
