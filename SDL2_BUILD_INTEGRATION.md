# SDL2 Graphics Integration - Build Setup Guide

## Overview

This document describes how to integrate SDL2 and OpenGL libraries into the HoverRace project files.

## Files Modified

1. **VideoServices/VideoBuffer.cpp** - Added SDL2Graphics backend support
2. **VideoServices/VideoBufferSDL2Integration.h** - New header for SDL2 integration layer
3. **VideoServices/VideoBufferSDL2Integration.cpp** - New implementation for SDL2 integration layer

## New Directories

- **NetTarget/GraphicsSDL2/** - Contains SDL2Graphics backend implementation
  - `SDL2Graphics.h` - Core graphics backend class
  - `SDL2Graphics.cpp` - OpenGL rendering implementation
  - `VideoBufferSDL2Adapter.h` - Adapter for backward compatibility
  - `VideoBufferSDL2Adapter.cpp` - Adapter implementation
  - `README.md` - Graphics module documentation

## Step 1: Add SDL2 & OpenGL Include Paths

### For NetTarget.vcxproj (Main Game Project)

1. Open `NetTarget.vcxproj` in Visual Studio
2. Go to **Project Properties → VC++ Directories**
3. Add to **Include Directories**:
   ```
   C:\vcpkg\installed\x64-windows\include
   C:\vcpkg\installed\x64-windows\include\SDL2
   C:\vcpkg\installed\x64-windows\include\glm
   ```

### For MazeCompiler.vcxproj

1. Open `MazeCompiler.vcxproj` in Visual Studio
2. Go to **Project Properties → VC++ Directories**
3. Add same include paths as above

## Step 2: Add SDL2 & OpenGL Library Paths

### For NetTarget.vcxproj

1. Go to **Project Properties → VC++ Directories**
2. Add to **Library Directories**:
   ```
   C:\vcpkg\installed\x64-windows\lib
   C:\vcpkg\installed\x64-windows\debug\lib
   ```

### For MazeCompiler.vcxproj

1. Same configuration as NetTarget.vcxproj

## Step 3: Link SDL2 & OpenGL Libraries

### For NetTarget.vcxproj

1. Go to **Project Properties → Linker → Input**
2. Add to **Additional Dependencies**:
   ```
   SDL2.lib
   SDL2main.lib
   OpenGL32.lib
   glew32.lib
   ```

### For MazeCompiler.vcxproj

1. Same libraries as NetTarget.vcxproj

## Step 4: Add GraphicsSDL2 Project to Solution

### Option A: Add Existing Files Directly

1. In Solution Explorer, right-click on solution
2. Select **Add → Existing Project**
3. Navigate to `NetTarget/GraphicsSDL2/`
4. Create a new **Static Library (.lib)** project with these files:
   - `SDL2Graphics.h`
   - `SDL2Graphics.cpp`
   - `VideoBufferSDL2Adapter.h`
   - `VideoBufferSDL2Adapter.cpp`

### Option B: Add Files to VideoServices Project

Alternatively, add the 4 files directly to the **VideoServices** project:

1. Right-click on VideoServices project
2. Select **Add → Existing Item**
3. Select all 4 files from `GraphicsSDL2/` directory
4. Click **Add**

## Step 5: Install SDL2 via VCPKG (if not already installed)

```bash
vcpkg install sdl2:x64-windows
vcpkg install glew:x64-windows
vcpkg install glm:x64-windows
```

If you don't have vcpkg installed:

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install sdl2:x64-windows glew:x64-windows glm:x64-windows
```

## Step 6: Verify Include and Library Paths

Check that the following paths exist:

```
C:\vcpkg\installed\x64-windows\include\SDL2\SDL.h
C:\vcpkg\installed\x64-windows\lib\SDL2.lib
C:\vcpkg\installed\x64-windows\lib\OpenGL32.lib
```

If paths are different, update the vcxproj paths accordingly.

## Step 7: Rebuild the Solution

1. Close Visual Studio
2. Delete `Release/` and `Debug/` folders in workspace
3. Open Visual Studio
4. **Build → Clean Solution**
5. **Build → Rebuild Solution**

## Troubleshooting

### SDL2.h not found

- Check **Project Properties → VC++ Directories → Include Directories**
- Verify vcpkg installation: `vcpkg list` should show SDL2

### SDL2.lib not found

- Check **Project Properties → Linker → Input → Additional Dependencies**
- Verify lib path in **Project Properties → VC++ Directories → Library Directories**
- For debug builds, add path to `debug/lib` folder

### OpenGL errors at runtime

- Ensure **OpenGL32.lib** is linked (graphics driver must support OpenGL 3.3+)
- Most modern Windows systems support this natively

### Link errors with SDL2main

- Remove `SDL2main.lib` if linking fails
- Use `SDL2.lib` only (the main will be handled by SDL2 macro)

## Verification

After building successfully:

1. Run the game executable
2. Look for "SDL2Graphics initialized successfully" in debug output
3. Game window should appear with graphics rendering
4. Frame rate should be 60 FPS or higher

## Project File Template (vcxproj)

For reference, here's what the modified section should look like:

```xml
<PropertyGroup Label="UserMacros" />
<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
  <ClCompile>
    <AdditionalIncludeDirectories>
      C:\vcpkg\installed\x64-windows\include;
      C:\vcpkg\installed\x64-windows\include\SDL2;
      %(AdditionalIncludeDirectories)
    </AdditionalIncludeDirectories>
  </ClCompile>
  <Link>
    <AdditionalLibraryDirectories>
      C:\vcpkg\installed\x64-windows\lib;
      %(AdditionalLibraryDirectories)
    </AdditionalLibraryDirectories>
    <AdditionalDependencies>
      SDL2.lib;SDL2main.lib;OpenGL32.lib;glew32.lib;
      %(AdditionalDependencies)
    </AdditionalDependencies>
  </Link>
</ItemDefinitionGroup>
```

## Next Steps

After build succeeds:

1. ✅ Test SDL2Graphics initialization
2. ✅ Run game with graphics rendering enabled
3. ✅ Verify palette colors display correctly
4. ✅ Test extended gameplay sessions
5. ✅ Performance benchmarking

See README.md in GraphicsSDL2 for graphics backend details.
