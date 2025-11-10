# HoverRace Build Instructions

## Environment
- **OS**: Windows
- **Shell**: PowerShell (pwsh.exe)
- **Build Tool**: MSBuild from Visual Studio 2022 Community
- **Platform**: Win32 (x86)
- **Configuration**: Release

## MSBuild Path
```
C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe
```

## Build Commands

### Quick Build (Game2 only - the main executable)
```powershell
cd c:\originalhr\HoverRace
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\Game2\Game2.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32"
```

### Build Individual Projects
```powershell
# Build Model (dependencies)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\Model\Model.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32"

# Build MainCharacter 
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\MainCharacter\MainCharacter.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32"

# Build Game2 (depends on all others)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\Game2\Game2.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32"
```

### View Only Last N Lines of Build Output
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" "c:\originalhr\HoverRace\NetTarget\Game2\Game2.vcxproj" "/p:Configuration=Release" "/p:Platform=Win32" 2>&1 | Select-Object -Last 10
```

## Output Location
Built executable: `c:\originalhr\HoverRace\Release\Game2.exe`

All DLL dependencies are also output to: `c:\originalhr\HoverRace\Release\`

## Running the Game
```powershell
cd c:\originalhr\HoverRace\Release
.\Game2.exe
```

### Run in Background (for testing)
```powershell
cd c:\originalhr\HoverRace\Release
Start-Process .\Game2.exe -WindowStyle Hidden
Start-Sleep -Seconds 30  # Wait for logging
```

## Logging Files Generated
When running Game2, the following logs are created in `c:\originalhr\HoverRace\Release\`:
- `Game2_TrackLoad.log` - Track/level loading info
- `Game2_MainLoop.log` - Main game loop execution
- `Game2_Actor_Render.log` - Actor rendering debug info
- `Game2_Character_Debug.log` - Character position/room tracking
- `Game2_Render_Entry.log` - Render pipeline entry logging
- `Game2_InsertElement.log` - Element insertion tracking
- `Game2_AddRenderer.log` - Renderer initialization
- `Game2_VideoBuffer_Lock.log` - Video buffer operations

## Clear All Logs
```powershell
cd c:\originalhr\HoverRace\Release
rm -Force Game2_*.log
```

## Key Project Files
- **Main Executable**: `NetTarget/Game2/Game2.vcxproj`
- **Character Model**: `NetTarget/MainCharacter/MainCharacter.vcxproj`
- **Game Logic**: `NetTarget/Model/Model.vcxproj`
- **Graphics**: `NetTarget/VideoServices/VideoServices.vcxproj`
- **Utilities**: `NetTarget/Util/Util.vcxproj`

## Known Issues & Fixes Applied

### Issue: AddRenderer() throws exception
**Status**: ✅ FIXED
- **Cause**: `MR_DllObjectFactory::CreateObject()` throws SEH exception
- **Fix**: Wrap in `__try/__except` block in `MainCharacter.cpp`
- **Result**: Character properly inserted into level, now renders

### Issue: Character position garbage data
**Status**: ✅ FIXED
- **Cause**: Uninitialized or corrupted position memory
- **Fix**: Added position bounds validation in `Observer.cpp` Render3DView()
- **Result**: Invalid positions detected and rendering skipped safely

### Issue: InsertElement silent failure
**Status**: ✅ FIXED
- **Cause**: Exception caught but ignored, character never added to actor list
- **Fix**: Added `__try/__except` in `AddRenderer()`, detailed logging in `InsertElement()`
- **Result**: Character now successfully inserted and visible

## Build Tips

1. **Always clean logs before testing**: `rm -Force Game2_*.log`
2. **Check for compilation errors**: Look for "Error(s)" in build output
3. **Warnings are OK**: Many warnings are harmless (deprecated MSVC functions)
4. **Rebuild takes ~3 seconds**: Quick turnaround for testing changes
5. **Use `-Last 10` flag** to see just the summary of build results
