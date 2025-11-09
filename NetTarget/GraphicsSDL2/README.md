# SDL2 Graphics Backend for HoverRace

This module provides modern SDL2 + OpenGL graphics rendering for HoverRace, replacing the deprecated DirectDraw implementation.

## Architecture

### Components

1. **SDL2Graphics.h/cpp** - Core graphics backend using SDL2 window management and OpenGL rendering
   - Handles 8-bit paletted buffer display
   - Manages OpenGL shaders and textures
   - Provides palette lookup via texture sampling

2. **VideoBufferSDL2Adapter.h/cpp** - Adapter layer for integration
   - Maintains backward compatibility with existing VideoBuffer interface
   - Minimal changes needed to game code

## Features

- **SDL2 Window Management** - Cross-platform windowing (Windows/Linux/Mac)
- **OpenGL 3.3 Core** - Modern graphics API
- **8-bit Palette Support** - Preserves original game's 256-color palette mode
- **Texture-based Rendering** - Uses GPU texture sampling for palette lookup
- **VSync Support** - Smooth 60 FPS rendering
- **Nearest-neighbor Filtering** - Retro look preserved

## Integration Steps

### 1. Replace VideoBuffer with SDL2 Adapter

In `VideoBuffer.cpp`, the `SetVideoMode()` function can use the adapter instead of DirectDraw:

```cpp
// Old: DirectDraw initialization
// BOOL lReturnValue = InitDirectDraw();

// New: SDL2 initialization  
VideoBufferSDL2Adapter* adapter = new VideoBufferSDL2Adapter();
BOOL lReturnValue = adapter->Initialize(mWindow, mXRes, mYRes);
```

### 2. Update Lock/Unlock Operations

Replace DirectDraw surface locking with adapter calls:

```cpp
// Old: mBackBuffer->Lock(...)
uint8_t* buffer;
if (adapter->Lock(buffer)) {
    // Render to buffer
    adapter->Unlock();
}
```

### 3. Set Game Palette

The adapter handles palette conversion automatically:

```cpp
// Set RGB palette (768 bytes: 256 colors * 3 bytes)
adapter->SetPalette(paletteData);
```

## Building

### Dependencies

- SDL2 (libSDL2-2.0)
- OpenGL 3.3+
- C++11 or later

### Windows

```bash
vcpkg install sdl2 glew
```

Add include paths:
- SDL2 include directory
- OpenGL include directory

Add library paths and link:
- SDL2.lib
- OpenGL32.lib

### Linux

```bash
sudo apt-get install libsdl2-dev libgl1-mesa-dev
```

## Performance

- **Target FPS**: 60+ (with VSync)
- **GPU Memory**: ~2MB for texture storage
- **CPU Usage**: Minimal (palette lookup via GPU)

## Migration Benefits

1. **Cross-platform** - Runs on Windows/Linux/Mac
2. **Future-proof** - Uses modern graphics APIs
3. **Better Performance** - GPU-accelerated rendering
4. **Backward Compatible** - Minimal game code changes
5. **Easier Debugging** - Modern graphics tools support

## Known Limitations

- Requires OpenGL 3.3+ capable GPU (widely available)
- SDL2 event handling replaces Windows message loop (affects input handling)
- Palette must be RGB format (not RGBX or BGRA)

## Future Enhancements

1. **GLSL Shader Effects** - Add post-processing effects
2. **Texture Scaling** - Support for higher resolution rendering
3. **Multi-window Support** - Split-screen rendering
4. **Input Abstraction** - SDL2-based input handling
5. **Audio Integration** - SDL2_mixer for sound

## Testing Checklist

- [ ] SDL2 window creates successfully
- [ ] OpenGL context initializes
- [ ] Shaders compile without errors
- [ ] 8-bit buffer displays correctly
- [ ] Palette changes apply immediately
- [ ] Game runs at 60 FPS
- [ ] No memory leaks
- [ ] Cross-platform compilation works
