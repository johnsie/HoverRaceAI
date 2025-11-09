# SDL2 Graphics Integration - Complete Setup Wizard

## Quick Start Guide

This guide will walk you through setting up the SDL2 graphics backend for HoverRace.

### Prerequisites

1. **Visual Studio 2022 or 2019** with C++ development tools
2. **Git** (for vcpkg)
3. **Windows 10/11** (or Windows 7+)

---

## Step 1: Install VCPKG and SDL2

### Option A: Automatic Installation (Recommended)

```powershell
# Open PowerShell as Administrator
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install sdl2:x64-windows glew:x64-windows glm:x64-windows
```

### Option B: Manual Installation

1. Clone vcpkg: `git clone https://github.com/Microsoft/vcpkg.git`
2. Run bootstrap: `.\bootstrap-vcpkg.bat`
3. Install libraries:
   ```
   vcpkg\vcpkg install sdl2:x64-windows
   vcpkg\vcpkg install glew:x64-windows
   vcpkg\vcpkg install glm:x64-windows
   ```

### Verify Installation

```
dir C:\vcpkg\installed\x64-windows\include\SDL2\SDL.h
dir C:\vcpkg\installed\x64-windows\lib\SDL2.lib
```

If both files exist, vcpkg is correctly installed ✓

---

## Step 2: Configure Project Files

### Option A: Automatic Configuration (Python Script)

```bash
# From HoverRace root directory
cd C:\originalhr\HoverRace
python update_projects_for_sdl2.py

# Or specify custom vcpkg path
python update_projects_for_sdl2.py --vcpkg-path C:\vcpkg\installed\x64-windows
```

The script will automatically update:
- `NetTarget/NetTarget.vcxproj`
- `NetTarget/MazeCompiler/MazeCompiler.vcxproj`

### Option B: Manual Configuration

Follow the detailed steps in `SDL2_BUILD_INTEGRATION.md`:
- Add SDL2 include paths
- Add library directories
- Link SDL2/OpenGL libraries

---

## Step 3: Verify Project Configuration

### Check Include Paths

In Visual Studio:
1. Right-click on `NetTarget` project → Properties
2. Go to **VC++ Directories**
3. Verify **Include Directories** contains:
   - `C:\vcpkg\installed\x64-windows\include`
   - `C:\vcpkg\installed\x64-windows\include\SDL2`

### Check Library Links

1. Go to **Linker → Input**
2. Verify **Additional Dependencies** contains:
   - `SDL2.lib`
   - `SDL2main.lib`
   - `OpenGL32.lib`
   - `glew32.lib`

---

## Step 4: Build the Solution

### Option A: Command Line Build

```bash
# From HoverRace root directory
.\build_sdl2.bat

# Or with msbuild directly
msbuild NetTarget.sln /p:Configuration=Release /p:Platform=x64 /t:Rebuild
```

### Option B: Visual Studio GUI Build

1. Open `NetTarget.sln` in Visual Studio
2. **Build → Clean Solution**
3. **Build → Rebuild Solution**
4. Wait for build to complete

### Expected Output

**Success indicators:**
```
========== Rebuild All: X succeeded, 0 failed ==========
```

**Common errors and solutions:**

| Error | Solution |
|-------|----------|
| `SDL2.h not found` | Check vcpkg include path in Project Properties |
| `Cannot find SDL2.lib` | Verify Library Directories includes vcpkg lib folder |
| `OpenGL32.lib not found` | Add OpenGL32.lib to Additional Dependencies |
| `LNK1104 cannot open file` | Verify all library paths in Additional Library Directories |

---

## Step 5: Runtime Setup

### Copy Required DLLs

After successful build, copy SDL2 DLL to executable directory:

```bash
# From HoverRace root
copy C:\vcpkg\installed\x64-windows\bin\SDL2.dll Release\
copy C:\vcpkg\installed\x64-windows\bin\glew32.dll Release\
```

Or set environment variable:
```bash
set PATH=%PATH%;C:\vcpkg\installed\x64-windows\bin
```

---

## Step 6: Run the Game

### Launch Game

```bash
# From HoverRace root
cd Release
NetTarget.exe
```

### Verify SDL2Graphics Activation

In the game window or debug output, look for:
```
SDL2Graphics initialized successfully
Lock: SDL2Graphics mode - requesting buffer
```

If you see these messages, the graphics backend is active! ✓

---

## Step 7: Troubleshooting

### SDL2 Window Not Appearing

**Symptom:** Game crashes immediately

**Solution:**
1. Verify SDL2.dll is in executable directory
2. Check for error messages in debug output
3. Run in Debug configuration for more details

### Graphics Rendering Issues

**Symptom:** Black screen or corrupted graphics

**Solution:**
1. Verify OpenGL 3.3+ support (Update GPU drivers)
2. Check palette initialization in debug output
3. Fall back to GDI mode (automatic if SDL2 fails)

### Build Failures

**Symptom:** MSBuild errors during compilation

**Solution:**
1. Run `python update_projects_for_sdl2.py` again
2. Verify vcpkg paths match your system
3. Clean build cache: `build_sdl2.bat` option 4
4. Check SDL2_BUILD_INTEGRATION.md for manual steps

### DLL Not Found at Runtime

**Symptom:** `SDL2.dll not found` or similar error

**Solution:**
```bash
# Copy DLLs to executable directory
copy C:\vcpkg\installed\x64-windows\bin\SDL2.dll Release\
copy C:\vcpkg\installed\x64-windows\bin\glew32.dll Release\
```

---

## Advanced Configuration

### Custom VCPKG Path

If vcpkg is installed elsewhere:

```bash
python update_projects_for_sdl2.py --vcpkg-path C:\my\custom\path\to\vcpkg\installed\x64-windows
```

### Building for Debug

```bash
msbuild NetTarget.sln /p:Configuration=Debug /p:Platform=x64 /t:Rebuild
```

### Static Linking (Optional)

To statically link SDL2 and OpenGL:

1. Build vcpkg libraries as static:
   ```bash
   vcpkg install sdl2:x64-windows --options shared
   ```

2. Update project to link static libraries

---

## What Gets Installed

### vcpkg Libraries

- **SDL2** - Window management, events, graphics context
- **GLEW** - OpenGL extension loader
- **GLM** - Math library (optional, for future use)

### HoverRace Files

- **GraphicsSDL2/** - New graphics backend
  - SDL2Graphics.h/cpp - OpenGL implementation
  - VideoBufferSDL2Adapter.h/cpp - Backward compatibility layer

- **VideoServices/** - Enhanced existing module
  - VideoBuffer.cpp - Integrated SDL2 support
  - VideoBufferSDL2Integration.h/cpp - Integration layer

- **Documentation**
  - SDL2_BUILD_INTEGRATION.md - Detailed build guide
  - SDL2_INTEGRATION_SUMMARY.md - Project overview
  - GraphicsSDL2/README.md - Graphics API reference

---

## Testing

### Automated Testing

After successful build:

```bash
# Run the game with graphics
cd Release
NetTarget.exe

# Expected:
# 1. Window opens
# 2. Game initializes without crashes
# 3. Frame rate displays 60+ FPS
# 4. Graphics render correctly
```

### Manual Testing Checklist

- [ ] Game launches without crashing
- [ ] SDL2Graphics initializes (debug output)
- [ ] Window displays with correct resolution
- [ ] Graphics render without artifacts
- [ ] Game runs at 60 FPS (VSync limited)
- [ ] Palette colors display correctly
- [ ] Game is playable for extended sessions
- [ ] No memory leaks detected

---

## Next Steps

After successful setup and testing:

1. **Enable Graphics Rendering**
   - Uncomment Observer rendering calls in GameApp.cpp
   - See RefreshView() in Game2/GameApp.cpp around line 1373

2. **Optimize Performance**
   - Profile with GPU debugger
   - Fine-tune shader code if needed

3. **Add Input Handling**
   - Integrate SDL2 event system
   - Map keyboard/mouse events

4. **Cross-Platform Testing**
   - Build on Linux with SDL2
   - Test macOS compatibility

---

## Support Resources

- **SDL2 Documentation**: https://wiki.libsdl.org/
- **OpenGL Tutorials**: https://learnopengl.com/
- **GLEW Documentation**: http://glew.sourceforge.net/
- **vcpkg Guide**: https://vcpkg.io/

---

## Quick Reference

| Task | Command |
|------|---------|
| Install vcpkg | `git clone https://github.com/Microsoft/vcpkg.git && cd vcpkg && bootstrap-vcpkg.bat` |
| Install SDL2 | `vcpkg install sdl2:x64-windows` |
| Update projects | `python update_projects_for_sdl2.py` |
| Build solution | `build_sdl2.bat` |
| Run game | `Release\NetTarget.exe` |
| Clean build | `msbuild NetTarget.sln /t:Clean` |

---

## Success Checklist

After completing all steps:

- [x] vcpkg installed with SDL2, GLEW, GLM
- [x] Project files updated with SDL2/OpenGL libraries
- [x] Solution builds without errors
- [x] Executable runs without crashing
- [x] SDL2Graphics backend initializes
- [x] Graphics render at 60+ FPS
- [x] Game is playable

**If all items are checked, the SDL2 graphics integration is complete and working!** ✅

---

**Questions or issues?** Refer to SDL2_BUILD_INTEGRATION.md for detailed troubleshooting.
