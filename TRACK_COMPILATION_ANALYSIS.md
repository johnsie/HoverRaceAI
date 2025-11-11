# HoverCad Track Compilation Analysis

## Executive Summary

The HoverRace track compilation pipeline has been fully analyzed and documented. The grayscale appearance of tracks in the current game is due to palette data embedded at **compile time**, not a graphics rendering bug.

## Architecture Overview

```
┌─────────────────┐
│  User opens     │
│  ClassicH.TR    │
│  in HoverCad    │
└────────┬────────┘
         │
         ▼
┌─────────────────────────────────┐
│  HoverCad GUI                   │
│  - Displays track in 2D editor  │
│  - User can modify geometry     │
│  - User clicks "Compile" button │
└────────┬────────────────────────┘
         │
         ▼
┌──────────────────────────────────────────┐
│  GenerateOutputFile() function           │
│  - Reads HCPolygon objects               │
│  - Reads HCAnchorWall objects            │
│  - Writes structured text format         │
│  - Creates temporary HC*.tmp file        │
│  - Saves debug copy HC*.tmp.debug        │
└────────┬─────────────────────────────────┘
         │
         ▼
┌──────────────────────────────────────────┐
│  MazeCompiler.exe                        │
│  - Reads temporary .txt file             │
│  - Reads ColorTab.pal palette (CRITICAL) │
│  - Parses track structure                │
│  - Embeds palette in binary output       │
│  - Writes final .trk file                │
└────────┬─────────────────────────────────┘
         │
         ▼
┌──────────────────────────────────────────┐
│  ClassicH.trk (compiled binary)          │
│  - Contains track geometry               │
│  - Contains EMBEDDED palette data        │
│  - Ready for Game2.exe to load           │
└──────────────────────────────────────────┘
```

## Key Finding: Palette Embedding

**The critical moment is during MazeCompiler execution:**

```
MazeCompiler <output.trk> <input.txt>
    ↓
    Reads: <input.txt> (track geometry)
    Reads: ColorTab.pal (CURRENT PALETTE IN WORKING DIRECTORY)
    ↓
    Writes: <output.trk> (geometry + EMBEDDED PALETTE)
```

### Why This Matters

1. **Old tracks have embedded grayscale palette**
   - ClassicH.trk was compiled when ColorTab.pal was grayscale
   - Changing ColorTab.pal now doesn't affect old .trk files
   - The palette is "baked into" the .trk file

2. **New tracks can have color palette**
   - Recompile with current ColorTab.pal (which has colors)
   - New .trk file will have color palette embedded
   - Game will display colors instead of grayscale

## HoverCad Source Code Analysis

### GenerateOutputFile() Function
**Location**: HoverCad/HoverCadDoc.cpp, lines 763-1052

Generates the intermediate text format by:

1. **Writing [Header] section** (lines 763-783)
   - Description (with escaped newlines)
   - Background image path

2. **Writing [Room] sections** (lines 813-865)
   - Each room gets unique ID
   - Floor level, texture ID
   - Ceiling level, texture ID
   - Multiple wall definitions

3. **Writing [Feature] sections** (lines 823-851)
   - 3D features within rooms
   - Parent room reference
   - Same floor/ceiling/wall format as rooms

4. **Writing [Initial_Position] sections** (lines 916-945)
   - Starting positions for players
   - Team assignments (1-10)
   - Position and orientation

5. **Writing [Free_Element] sections** (lines 950-970)
   - Game objects on track
   - Power-ups, obstacles, etc.
   - Element type IDs

6. **Writing [Connection_List] section** (lines 977-1025)
   - Room connectivity graph
   - Which walls connect to adjacent rooms
   - Enables pathfinding and room transitions

### File Save Process
**Location**: HoverCad/HoverCadDoc.cpp, lines 1914-1990

```cpp
// 1. Create temporary file
GetTempPath(sizeof(lTempPath), lTempPath);
GetTempFileName(lTempPath, "HC", 0, lTempFileName);

// 2. Generate output to temporary file
FILE* lFile = fopen(lTempFileName, "w");
BOOL lSuccess = GenerateOutputFile(lFile);

// 3. Save debug copy for troubleshooting
char lDebugFileName[320];
strcpy(lDebugFileName, lTempFileName);
strcat(lDebugFileName, ".debug");
CopyFile(lTempFileName, lDebugFileName, FALSE);

// 4. Run MazeCompiler with shell command
CString lCommand = CString("MazeCompiler \"")+lDialog.GetPathName()+"\" \""+lTempFileName+"\"";
int lExitCode = system(lCommand);
```

This is valuable because:
- Debug copies are saved in `%TEMP%\HC*.tmp.debug`
- These can be reused without opening HoverCad again
- The text format is identical to what HoverCad generates

## Intermediate Text Format Specification

### Complete Example

```
[Header]
Description=Level: Easy\nLength: 0.6 km\nShort and fast track\n
Background=C:\originalhr\HoverRace\NetTarget\mazes\BG-CITY.pcx

[Room]
Id=1
floor= 0.000000, 1, 51
ceiling= 4.000000, 1, 50
wall=362.324000, 31.188000, 1, 52
wall=362.498000, 71.283000, 1, 69
wall=404.144000, 72.043000, 1, 53
wall=403.840000, 31.766000, 1, 52

[Room]
Id=2
floor= 0.000000, 1, 51
ceiling= 4.000000, 1, 50
wall=362.194000, 74.475000, 1, 52
...

[Initial_Position]
Section=1
Position= 392.591000, 56.610000, 0.000000
Orientation= 90.000000
Team= 1

[Free_Element]
Section=19
Position= 381.850000, 72.849000, 0.000000
Orientation= 0.000000
Element_Type= 1, 202

[Connection_List]
1, 1, 19, 2
1, 3, 18, 4
2, 1, 3, 4
```

### Format Details

**Numeric Values:**
- Heights (floor/ceiling): Floating point (game units, typically 1/1000 meter)
- Coordinates (walls, positions): Floating point (X, Y in horizontal plane)
- Angles: Floating point (degrees, 0-360)
- IDs: Integer (positive)
- Texture references: DLL ID, Type ID (two integers)

**Text Encoding:**
- UTF-8 or ASCII
- Newlines escaped as `\n` in descriptions
- Carriage returns converted from `\r`

**Section Ordering:**
1. [Header] - Must be first
2. [Room] and [Feature] - Can be interleaved
3. [Initial_Position] - After rooms/features
4. [Free_Element] - After initial positions
5. [Connection_List] - Must be last

## Current Grayscale Issue Analysis

### Root Cause Timeline

1. **Timeframe: 08/11/2025 10:45** - ClassicH.TR was last compiled
2. **At that moment:** MazeCompiler read a grayscale ColorTab.pal
3. **Result:** Grayscale palette embedded in ClassicH.trk
4. **Current state:** ColorTab.pal is corrected with colors, but ClassicH.trk still has embedded grayscale

### Why Palette Changes Don't Help

```
Game2.exe loads ClassicH.trk
    ↓
Reads embedded palette from .trk file (GRAYSCALE from old compilation)
    ↓
ColorTab.pal is ignored for this track
    ↓
Renders with grayscale palette
```

### Evidence in Code

**VideoBuffer.cpp, CreatePalette()** (lines 410-524):
- Reads palette from track file (if embedded)
- Or loads from ColorTab.pal (if not embedded)
- Passes palette to SDL2Graphics via SetPalette()

The `.trk` file has **priority** - if a palette is embedded, it's used.

## Solution Implementation

### Immediate (Recommended)
1. Open HoverCad
2. Load each .TR file from `NetTarget\mazes\`
3. Click Compile
4. Save to `Release\Tracks\mazes\`

Result: New .trk files with current color palette embedded.

### Alternative (Automation-Ready)
1. Use HoverCad debug files from `%TEMP%\HC*.tmp.debug`
2. Run: `MazeCompiler.exe output.trk debug_file.txt`
3. Result: Same as above, but scriptable

### Batch Processing
Use provided scripts:
- **Windows batch**: `compile_tracks.bat` (or `.cmd`)
- **PowerShell**: `compile_tracks.ps1`

Both scripts:
- Find HoverCad debug files
- Verify ColorTab.pal is available
- Run MazeCompiler for each debug file
- Generate new .trk files in proper location

## Technical Details

### MazeCompiler Parsing

**Location**: NetTarget/MazeCompiler/main.cpp, lines 250-310

```cpp
// MazeCompiler reads .txt file using Parser class
MR_Parser lParser(lFile);

// Parse sections in order
if (lParser.GetNextClass("HEADER") == NULL) {
    // Use defaults if missing
}

while (lParser.GetNextClass("ROOM") != NULL) {
    // Parse room data
}

while (lParser.GetNextClass("FEATURE") != NULL) {
    // Parse feature data
}

while (lParser.GetNextClass("INITIAL_POSITION") != NULL) {
    // Parse starting positions
}

while (lParser.GetNextClass("FREE_ELEMENT") != NULL) {
    // Parse game objects
}

lParser.GetNextClass("CONNECTION_LIST");
// Parse room connections
```

### Palette Handling

**ColorTab.pal format**:
- JASC-PAL format (Paintshop Pro palette)
- Text-based header with color count
- RGB triplets (one per line)
- 256 colors × 3 bytes = 768 bytes palette data

**Embedded palette in .trk**:
- Binary format
- Palette data baked into record structure
- Read by VideoBuffer when loading track
- Passed to graphics backend

## Impact Assessment

### Graphics Quality
- ✗ Current tracks: Grayscale rendering
- ✓ After recompilation: Color rendering
- ✓ Performance: No impact (same data, different colors)
- ✓ Gameplay: No impact (colors don't affect gameplay)

### File Size
- Recompiled .trk files: Approximately same size
- ColorTab.pal addition: ~768 bytes per track
- Overall: Negligible impact

### Compatibility
- Old .trk files: Will continue to work (with grayscale)
- New .trk files: Compatible with all Game2.exe versions
- Reversible: Can always recompile with different palette

## Documentation Deliverables

1. **TRACK_COMPILER_FORMAT.md** - Complete format specification
2. **TRACK_RECOMPILATION_GUIDE.md** - Step-by-step user guide
3. **compile_tracks.ps1** - PowerShell automation script
4. **compile_tracks.bat** - Windows batch automation script
5. **This document** - Technical analysis and architecture

## Recommendations

### Short-term (Next Session)
1. Recompile all tracks using HoverCad GUI
2. Verify colors display correctly in Game2.exe
3. Test gameplay with colored tracks

### Medium-term
1. Set up batch recompilation workflow
2. Integrate into build process
3. Document for future track modifications

### Long-term
1. Consider implementing direct .TR to .txt converter (Python)
2. Automate entire pipeline
3. Create track compilation toolchain documentation

## References

- **HoverCad/HoverCadDoc.cpp** - Source code showing GenerateOutputFile()
- **NetTarget/MazeCompiler/main.cpp** - Source code showing compilation
- **NetTarget/MazeCompiler/Parser.h/cpp** - Parser implementation
- **Debug files**: `%TEMP%\HC*.tmp.debug` - Example output format

## Conclusion

The grayscale appearance of tracks is **not a graphics rendering bug** but a **palette data versioning issue**. The solution is well-understood and straightforward: recompile the tracks with the current ColorTab.pal file. All necessary tools and scripts have been created to make this process simple and repeatable.
