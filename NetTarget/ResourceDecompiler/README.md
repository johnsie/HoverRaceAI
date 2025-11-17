# ResourceDecompiler

Reverse-engineering tool for HoverRace game resource libraries. Extracts binary compiled resources back into human-readable and editable formats.

## Purpose

The HoverRace engine uses a ResourceCompiler tool to package game assets (3D models, bitmaps, audio) into binary resource libraries. This tool reverses that process, enabling:

- **Asset Recovery**: Extract 3D models, bitmaps, and audio from compiled libraries
- **Modding**: Re-edit extracted .msh mesh files and recompile
- **Archival**: Preserve game assets in source-friendly formats
- **Analysis**: Study game asset structure and format

## Build

```powershell
msbuild ResourceDecompiler\ResourceDecompiler.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32
```

Output: `Release\ResourceDecompiler.exe`

## Usage

```cmd
ResourceDecompiler <input_library> <output_directory>
```

### Examples

```cmd
# Extract main game actors library
ResourceDecompiler actors.dat decompiled_actors/

# Extract bitmap resources
ResourceDecompiler textures.dat decompiled_textures/

# Extract all sounds
ResourceDecompiler sounds.dat decompiled_sounds/
```

## Output Formats

### Meshes (.msh)
- **Format**: Text-based HoverRace mesh format
- **Structure**: SEQUENCE → FRAME → PATCH → vertices/colors
- **Example**: `actor_5.msh` (boxer mesh)
- **Editable**: Yes - can edit and recompile with ResourceCompiler

### Bitmaps (.tga)
- **Format**: TGA (Tagged Image Format)
- **Editable**: Yes - use any image editor

### Audio (.wav)
- **Format**: WAV (Waveform Audio File)
- **Editable**: Yes - use any audio editor

### Sprites (.dat)
- **Format**: Raw sprite data (binary)
- **Editable**: Limited - format-specific tooling needed

## Architecture

### Decompilation Flow

```
Binary Library (.dat)
    ↓
Read Header (magic: 12345)
    ↓
Read Resource Directory
    ↓
For Each Resource:
  - Seek to data offset
  - Read binary data
  - Convert to target format
  - Write output file
    ↓
Output Directory (source files)
```

### Resource Types

| Type | ID | Output Format | Purpose |
|------|-------|---------------|---------|
| BITMAP | 1 | .tga | Textures, UI elements |
| ACTOR | 2 | .msh | 3D models, hovercraft, obstacles |
| SPRITE | 3 | .dat | 2D game sprites |
| SHORT_SOUND | 4 | .wav | Sound effects (< 1 sec) |
| CONT_SOUND | 5 | .wav | Continuous sounds, music |

## Implementation Status

### ✅ Completed
- Project structure and build setup
- Binary library header parsing (magic number, resource count)
- Resource directory reading
- Output directory creation
- Basic bitmap/sprite/sound extraction (raw format)
- Executable console tool with usage help

### 🔄 In Progress
- Proper WAV header generation from binary sound data
- Mesh format conversion (binary → .msh text)
- TGA image format conversion

### 📋 TODO
- Parse binary actor format to extract vertex data
- Convert vertices/colors to .msh SEQUENCE/FRAME/PATCH structure
- Proper TGA header generation
- WAV audio format parsing and conversion
- Error handling and validation
- Logging and diagnostics
- Support for resource name preservation

## File Structure

```
ResourceDecompiler/
├── ResourceDecompiler.h       # Main decompiler class
├── ResourceDecompiler.cpp     # Implementation
├── main.cpp                   # CLI entry point
├── ResourceDecompiler.vcxproj # Visual Studio project
└── README.md                  # This file
```

## Technical Notes

### Binary Format (ResourceLibBuilder)
- **Magic Number**: 12345 (int32) - validates file is resource library
- **Entry Count**: int32 - number of resources
- **Resource Directory**: For each resource:
  - Type (int32): RESOURCE_TYPE_*
  - ID (int32): Unique identifier
  - Offset (int32): File position of data
  - Length (int32): Data size in bytes
  - Name (256 bytes): Resource filename/path
- **Data Section**: Raw resource data at specified offsets

### .msh Format (HoverRace Mesh)
```
SEQUENCE
  FRAME 0
    PATCH 0
      # Vertices: X Y Z coordinates
      # Colors: MR_RED, MR_GREEN, etc.
      # Triangles: vertex indices
    PATCH_END
    PATCH 1
      ...
    PATCH_END
  FRAME_END
  FRAME 1
    ...
  FRAME_END
SEQUENCE_END
```

## Limitations

- Original filenames are lost (replaced with ID-based names: actor_5.msh)
- Bitmap format conversion may need refinement for exact pixel-perfect reproduction
- Audio quality depends on original compression format
- Sprite format is output as raw binary (requires format-specific tools to view)

## Related Tools

- **ResourceCompiler**: Compiles source assets into binary library (inverse operation)
- **ResActorBuilder**: Reads .msh files during compilation
- **ResBitmapBuilder**: Reads bitmap files during compilation
- **Game2.exe**: HoverRace game engine that uses these resources

## License

This tool is provided as-is for HoverRace modding and archival purposes.
