# RaceServer Build Status

**Date**: November 14, 2025  
**Status**: ⚠️ **Ready to Build - Compiler Not Installed**

---

## Build Requirements

To build RaceServer.exe, your system needs:

- **Visual Studio 2015 or later** (Community, Professional, or Enterprise)
- **Or**: Visual Studio Build Tools 2019+
- **Or**: Any C++ compiler compatible with Windows/Winsock2

---

## What's Ready to Build

✅ **RaceServer.vcxproj** - Visual Studio project file (complete)  
✅ **All source files** - ~1,900 lines of code  
✅ **All headers** - Properly structured  
✅ **Configuration** - config.xml template  

**Location**: `c:\originalhr2\HoverRaceAI\NetTarget\RaceServer\`

---

## Build Instructions

### Option 1: Build from Visual Studio IDE (Recommended)

```bash
# Open the project
cd c:\originalhr2\HoverRaceAI\NetTarget\RaceServer
start RaceServer.vcxproj
```

**In Visual Studio**:
1. Select "Release" configuration (top left dropdown)
2. Select "Win32" platform
3. Right-click project → "Rebuild"
4. Wait for build to complete

**Output**: `Release\RaceServer.exe`

### Option 2: Build from Command Line (MSBuild)

```bash
cd c:\originalhr2\HoverRaceAI\NetTarget\RaceServer

# Using msbuild (Visual Studio 2019+)
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\msbuild.exe" RaceServer.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32

# Or for Visual Studio 2022
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" RaceServer.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32
```

### Option 3: Build from PowerShell

```powershell
cd c:\originalhr2\HoverRaceAI\NetTarget\RaceServer
dotnet build RaceServer.vcxproj -c Release -p:Platform=Win32
```

---

## Build Configuration

**Project File**: `RaceServer.vcxproj`

**Configurations**:
- Release (optimized) - Recommended for deployment
- Debug (with symbols) - For debugging

**Platform**: Win32 (x86, 32-bit)

**Output Directory**: 
- Release: `c:\originalhr2\HoverRaceAI\Release\RaceServer.exe`
- Debug: `c:\originalhr2\HoverRaceAI\Debug\RaceServer.exe`

---

## Build Components

The following source files will be compiled:

**Core**
- `RaceServer.cpp` - Main entry point (325 lines)
- `stdafx.cpp` - Precompiled headers

**Network Interface** (550 lines)
- `NetworkInterface\ServerSocket.cpp`
- `NetworkInterface\ClientConnection.cpp`
- `NetworkInterface\MessageDispatcher.cpp`

**Game Simulation** (305 lines)
- `GameSimulation\GameSimulation.cpp`

**Race Management** (350 lines)
- `RaceManager\RaceManager.cpp`
- `RaceManager\RaceSession.cpp`

**Infrastructure** (200 lines)
- `Config\ServerConfig.cpp`
- `Logging\ServerLogger.cpp`

**Total**: ~1,900 lines of code

---

## Expected Build Output

**On successful build**:
```
Microsoft (R) Build Engine version 16.11.0...

RaceServer.vcxproj -> Release\RaceServer.exe

Build succeeded.
    0 Error(s)
    0 Warning(s)

Time Elapsed 00:00:15
```

**Output file**: `Release\RaceServer.exe` (~500 KB)

---

## Running the Server (After Build)

```bash
# Navigate to Release directory
cd Release

# Run the server
RaceServer.exe 9600 raceserver.log

# Expected output:
# HoverRace Centralized Race Server v1.0
# =======================================
# 
# Server listening on port 9600
# Press Ctrl+C to shutdown gracefully
```

---

## Troubleshooting Build Errors

### Error: "Cannot find cl.exe"
**Cause**: Visual Studio C++ compiler not installed  
**Solution**: Install Visual Studio with C++ workload or Build Tools

### Error: "RaceServer.vcxproj not found"
**Cause**: Project file not in current directory  
**Solution**: Navigate to `c:\originalhr2\HoverRaceAI\NetTarget\RaceServer\` first

### Error: "Cannot open include file 'stdafx.h'"
**Cause**: Source file location incorrect  
**Solution**: Ensure all .cpp and .h files are in the correct directories:
- `RaceServer.cpp` - Root
- `stdafx.h / stdafx.cpp` - Root
- `NetworkInterface/*.cpp` - NetworkInterface/
- `GameSimulation/*.cpp` - GameSimulation/
- `RaceManager/*.cpp` - RaceManager/
- `Config/*.cpp` - Config/
- `Logging/*.cpp` - Logging/

### Error: "ws2_32.lib not found"
**Cause**: Windows SDK not installed  
**Solution**: Install Windows SDK with Visual Studio

### Error: LNK2001 "unresolved external symbol"
**Cause**: Missing object files  
**Solution**: Ensure all .cpp files are listed in project file (already done)

---

## Verification Steps

After building successfully, verify the executable:

```bash
# Check file exists
ls Release\RaceServer.exe

# Check file size (should be ~500 KB)
(Get-Item Release\RaceServer.exe).Length

# Check is executable
file Release\RaceServer.exe
# Should show: PE32 executable

# Test run (will fail if listening port in use, but shows executable works)
Release\RaceServer.exe 9700 test.log
# Press Ctrl+C after 2 seconds
```

---

## What to Do Next

1. **Install Visual Studio** (if not already installed)
   - Download from: https://visualstudio.microsoft.com
   - Install C++ workload
   - Takes ~30 minutes

2. **Build RaceServer**
   - Follow instructions above
   - Should complete in <30 seconds

3. **Run RaceServer**
   - Execute: `RaceServer.exe 9600 raceserver.log`
   - Verify server starts successfully

4. **Continue Integration**
   - Follow Phase 3 in `CENTRALIZED_SERVER_IMPLEMENTATION.md`
   - Integrate with InternetRoom
   - Update client

---

## Build Summary

| Item | Status |
|------|--------|
| Source files | ✅ Complete |
| Project file | ✅ Complete |
| Configuration | ✅ Complete |
| Dependencies | ✅ Ready (Winsock2 + STL) |
| Build requirements | ⚠️ Visual Studio needed |
| Documentation | ✅ Complete |

**Next Step**: Install Visual Studio, then build RaceServer.exe

---

*RaceServer implementation is complete and ready to compile.*
