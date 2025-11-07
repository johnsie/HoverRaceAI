# HoverRace Architecture & Troubleshooting Guide

## System Architecture

### Core Library Dependencies

#### Tier 1: Foundation Libraries
**Util.dll** - Core utilities and infrastructure
- Location: `NetTarget/Util/`
- Dependencies: None (only system libraries)
- Exports:
  - `MR_RecordFile` - File I/O
  - `MR_DllObjectFactory` - Object factory pattern
  - `MR_Profiler` - Performance profiling
  - `MR_CoordinateSystem` - 2D/3D coordinate transforms
  - Utility templates: `MR_FastArray`, `MR_FastFifo`, etc.
- Key Files:
  - `RecordFile.h/cpp` - Binary file format handling
  - `WorldCoordinates.h` - Coordinate transformation
  - `Profiler.h` - Profiling infrastructure

#### Tier 2: Rendering & Audio Services
**VideoServices.dll** - 3D rendering and DirectSound
- Location: `NetTarget/VideoServices/`
- Dependencies: Util.dll
- Exports:
  - `MR_VideoBuffer` - 3D viewport rendering
  - `MR_SoundServer` - Audio playback
  - `MR_ColorPalette` - Color management
  - `MR_Sprite` - 2D sprite rendering
- Key Files:
  - `VideoBuffer.h/cpp` - DirectDraw wrapper (disabled)
  - `SoundServer.h/cpp` - DirectSound audio
  - `3DViewport.h/cpp` - 3D rendering pipeline
  - `Patch.h/cpp` - Texture patching

#### Tier 3: Game Logic
**Model.dll** - Game world and physics simulation
- Location: `NetTarget/Model/`
- Dependencies: Util.dll, VideoServices.dll
- Exports:
  - `MR_Level` - Game level/track
  - `MR_MazeElement` - Level elements
  - `MR_GameSession` - Game session management
  - `MR_PhysicalCollision` - Physics system
- Key Files:
  - `Level.h/cpp` - Track/level management
  - `PhysicalCollision.h/cpp` - Collision detection
  - `Shapes.h/cpp` - Geometric shapes

**MainCharacter.dll** - Player character logic
- Location: `NetTarget/MainCharacter/`
- Dependencies: Util.dll, Model.dll, VideoServices.dll
- Exports:
  - `MR_MainCharacter` - Player character
  - `MR_MainCharacterRenderer` - Character rendering
- Key Files:
  - `MainCharacter.h/cpp` - Player state and behavior
  - `MainCharacterRenderer.h/cpp` - Character rendering

#### Tier 4: Object Management
**ObjFacTools.dll** - Object factory and resource management
- Location: `NetTarget/ObjFacTools/`
- Dependencies: Util.dll, VideoServices.dll, Model.dll
- Exports:
  - `MR_DllObjectFactory` - Loadable object factory
  - `MR_ResourceLib` - Resource library
  - Resource loaders for: Actors, Bitmaps, Sprites, Sounds
- Key Files:
  - `ObjectFactoryData.h/cpp` - Object factory data
  - `ResourceLib.h/cpp` - Resource management
  - `ResBitmap.h/cpp`, `ResSprite.h/cpp`, etc. - Resource types

**ColorTools.dll** - Color palette utilities
- Location: `NetTarget/ColorTools/`
- Dependencies: Util.dll
- Exports:
  - `MR_ColorConverter` - Color space conversion
  - Palette utilities
- Key Files:
  - `ColorTools.h/cpp` - Color utilities

#### Tier 5: Applications
**Game2.exe** - Main game application
- Location: `NetTarget/Game2/`
- Dependencies: All core libraries + MainCharacter
- Key Files:
  - `GameApp.h/cpp` - Application framework
  - `ClientSession.h/cpp` - Client gameplay
  - `NetworkSession.h/cpp` - Network multiplayer
  - `Observer.h/cpp` - Game camera/view

**PaletteCreator.exe** - Palette creation tool
- Location: `NetTarget/PaletteCreator/`
- Dependencies: ColorTools.dll
- Standalone utility for palette management

---

## Compiler & Linker Configuration

### Preprocessor Definitions by Layer

```
TIER 1 (Util.dll):
  WIN32, _WINDOWS, _AFXDLL

TIER 2 (VideoServices.dll):
  WIN32, _WINDOWS, MR_VIDEO_SERVICES, _AFXDLL

TIER 3 (Model.dll):
  WIN32, _WINDOWS, MR_MODEL, _AFXDLL

TIER 3 (MainCharacter.dll):
  WIN32, _WINDOWS, MR_MAIN_CHARACTER, _AFXDLL

TIER 4 (ObjFacTools.dll):
  WIN32, _WINDOWS, MR_OBJ_FAC_TOOLS, _AFXDLL

TIER 4 (ColorTools.dll):
  WIN32, _WINDOWS, MR_COLOR_TOOLS, _AFXDLL

APPLICATIONS (Game2.exe, PaletteCreator.exe):
  WIN32, _WINDOWS, _AFXDLL (no module-specific defines)
```

### Include Paths (All Projects)

```
$(VisualStudioDir)\SDK\ScopeCppSDK\vc15\VC\include
$(VisualStudioDir)\VC\Tools\MSVC\14.39.33519\include
```

### Library Paths (All Projects)

```
c:\originalhr\HoverRace\Release     <!-- Built DLL import libraries -->
$(VisualStudioDir)\SDK\ScopeCppSDK\vc15\VC\lib\x86
$(VisualStudioDir)\VC\Tools\MSVC\14.39.33519\lib\x86
```

---

## Common Issues & Resolution

### Issue Category 1: Linking Errors

#### Problem: LNK2019 - Unresolved External Symbol

**Typical Error Message:**
```
error LNK2019: unresolved external symbol "__declspec(dllimport) ...MR_SoundServer::Play..."
```

**Root Causes:**
1. ❌ Missing ProjectReference dependency
2. ❌ Library name spelled incorrectly in AdditionalDependencies
3. ❌ Library path doesn't include built library directory
4. ❌ Wrong build configuration (Debug lib used in Release)

**Diagnosis Steps:**
```powershell
# Step 1: Verify library exists
ls c:\originalhr\HoverRace\Release\*.lib

# Step 2: Check library exports
$dumpbin = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\bin\Hostx86\x86\dumpbin.exe"
& $dumpbin /exports c:\originalhr\HoverRace\Release\Util.dll | grep -i "SoundServer"

# Step 3: Check .vcxproj for ProjectReference
Select-String "ProjectReference" NetTarget/Game2/Game2.vcxproj
```

**Solution Steps:**
1. Add ProjectReference if missing:
   ```xml
   <ItemGroup>
     <ProjectReference Include="..\VideoServices\VideoServices.vcxproj">
       <Project>{158869E8-53E8-4DC1-B41F-B1C996211E2B}</Project>
     </ProjectReference>
   </ItemGroup>
   ```

2. Verify AdditionalDependencies uses semicolons:
   ```xml
   <AdditionalDependencies>vfw32.lib;wsock32.lib;VideoServices.lib;%(AdditionalDependencies)</AdditionalDependencies>
   ```

3. Check AdditionalLibraryDirectories includes Release folder:
   ```xml
   <AdditionalLibraryDirectories>c:\originalhr\HoverRace\Release;</AdditionalLibraryDirectories>
   ```

---

### Issue Category 2: Compilation Errors

#### Problem: C2065 - Undeclared Identifier

**Typical Error Message:**
```
error C2065: 'lCounter': undeclared identifier
```

**Root Cause:**
C++11+ scopes variables declared in `for(int x=...)` to loop body only

**Example:**
```cpp
// Lines 806-815 (NetworkSession.cpp)
// ❌ WRONG: lCounter goes out of scope after first loop
for( int lCounter = 0; lCounter < 10; lCounter++ ) { ... }

// Later in function:
for( lCounter = 0; lCounter < 10; lCounter++ ) { ... }  // ERROR: undeclared

// ✅ CORRECT: Declare outside loop
int lCounter;
for( lCounter = 0; lCounter < 10; lCounter++ ) { ... }
for( lCounter = 0; lCounter < 10; lCounter++ ) { ... }  // OK
```

**Files with This Issue:**
- `NetTarget/Game2/NetworkSession.cpp`
  - Line 806 (first declaration)
  - Line 903 (reuse)
  - Line 1067 (reuse)

---

#### Problem: C1189 - MFC Build Error

**Typical Error Message:**
```
error C1189: #error: Building MFC application with /MD[d] (CRT dll version) 
requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]
```

**Root Cause:**
- Using `/MD` or `/MDd` (dynamic CRT) without `_AFXDLL` preprocessor define
- MFC requires explicit dynamic linking declaration

**Solution:**
Add `_AFXDLL` to PreprocessorDefinitions:

**Fix Location**: `ResourceCompiler.vcxproj`
```xml
<PropertyGroup Label="Configuration" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
  <UseOfMfc>Dynamic</UseOfMfc>
  <!-- Now also need to set CharacterSet to MultiByte -->
  <CharacterSet>MultiByte</CharacterSet>
</PropertyGroup>

<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
  <ClCompile>
    <!-- Add _AFXDLL to definitions -->
    <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;_AFXDLL;%(PreprocessorDefinitions)</PreprocessorDefinitions>
  </ClCompile>
</ItemDefinitionGroup>
```

---

#### Problem: C3867 - Non-standard Member Function Syntax

**Typical Error Message:**
```
error C3867: 'CFile::AssertValid': non-standard syntax; use '&' to create a pointer to member
```

**Root Cause:**
Attempting to reference member function without calling it

**Example:**
```cpp
// ❌ WRONG: Not calling the function
CFile::AssertValid;

// ✅ CORRECT: Call the member function
CFile::AssertValid();
```

**Fix Location**: `RecordFile.cpp:577`

---

### Issue Category 3: Header & Include Issues

#### Problem: C1083 - Cannot Open Include File

**Typical Error Message:**
```
error C1083: Cannot open include file: 'afx.h': No such file or directory
```

**Root Cause:**
Missing MFC headers in include path or not included in source

**Solution:**
Add explicit MFC include at top of files using MFC:

**Files Fixed:**
- `WorldCoordinates.h` - Added `#include <afx.h>`
- `Patch.cpp` - Added `#include "Patch.h"`
- `SoundServer.cpp` - Added proper header chain

---

## Performance Optimization Points

### Build Time Optimization

**Current Build Times:**
- Full Debug build: ~8-10 seconds
- Full Release build: ~16-18 seconds
- Incremental build: ~0.5-2 seconds

**Optimization Opportunities:**
1. **Parallel Builds**: Already using `/m:1` (single-threaded for stability)
   - Can try `/m:4` or `/m:8` for multi-core systems
   - Monitor for linker contention

2. **Precompiled Headers**: Already using `StdAfx.h` in all projects
   - Further optimization: Move more stable includes there

3. **Link Time Code Generation (LTCG)**: Disabled for faster builds
   - Release optimizations already include `/O2`

---

## Runtime Debugging

### Debug Symbols & PDB Files

**Location:** Each project's `Release/` folder
```
c:\originalhr\HoverRace\NetTarget\Util\Release\Util.pdb
c:\originalhr\HoverRace\NetTarget\VideoServices\Release\VideoServices.pdb
... etc
```

**Debugging Setup:**
1. Keep `.pdb` files alongside `.dll`/`.exe`
2. In Visual Studio: Debug → Modules (shows loaded PDBs)
3. Configure symbol paths in Tools → Options → Debugging → Symbols

### Profiling Support

**Util.dll Profiler:**
- Location: `NetTarget/Util/Profiler.h`
- Class: `MR_ProfilerSampler`
- Usage: Wrap code sections for timing
- Output: Performance metrics to debug log

---

## File Organization

### Source Code Layout

```
NetTarget/
├── Util/                 Tier 1: Core utilities
│   ├── RecordFile.h/cpp     Binary I/O
│   ├── Profiler.h/cpp       Performance profiling
│   ├── WorldCoordinates.h   Coordinate transforms
│   ├── FastArray.h          Template utilities
│   └── ...
│
├── VideoServices/        Tier 2: Rendering & Audio
│   ├── VideoBuffer.h/cpp    3D viewport
│   ├── SoundServer.h/cpp    Audio playback
│   ├── Patch.h/cpp          Texture management
│   └── ...
│
├── Model/                Tier 3: Game World
│   ├── Level.h/cpp          Track/level management
│   ├── MazeElement.h/cpp    Level elements
│   ├── PhysicalCollision.h/cpp Physics
│   └── ...
│
├── MainCharacter/        Tier 3: Player Character
│   ├── MainCharacter.h/cpp
│   └── MainCharacterRenderer.h/cpp
│
├── ObjFacTools/          Tier 4: Object Management
│   ├── ObjectFactoryData.h/cpp
│   ├── ResourceLib.h/cpp
│   └── Res*.h/cpp            (Resource loaders)
│
├── ColorTools/           Tier 4: Color Management
│   └── ColorTools.h/cpp
│
├── Game2/                Tier 5: Main Game
│   ├── GameApp.h/cpp         Application
│   ├── ClientSession.h/cpp   Gameplay
│   ├── NetworkSession.h/cpp  Multiplayer
│   └── ...
│
└── PaletteCreator/       Tier 5: Tools
    └── PaletteCreator.h/cpp
```

---

## Testing Strategy

### Unit Testing Points

**Module Tests (Build Level):**
- ✅ Util.dll exports all symbols
- ✅ VideoServices.dll links to Util
- ✅ Model.dll links to Util + VideoServices
- ✅ MainCharacter.dll links to all dependencies
- ✅ Game2.exe links to all libraries

**Functional Tests (Runtime Level):**
- [ ] Game2.exe launches without errors
- [ ] Tracks load correctly
- [ ] Player character initialized
- [ ] Rendering pipeline active
- [ ] Audio system responsive
- [ ] Network connectivity (if enabled)

---

## Deployment Checklist

- [ ] All DLLs present in deployment directory
- [ ] PDB files included for debugging
- [ ] Runtime dependencies (MFC DLL) available
- [ ] Required fonts/resources present
- [ ] Windows SDK libraries accessible
- [ ] Test on clean machine without VS installed

---

