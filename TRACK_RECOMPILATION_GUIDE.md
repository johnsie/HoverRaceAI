# Track Recompilation Guide: How to Get Colors Back

## Problem Summary

The track files (ClassicH.trk, etc.) were compiled when the `ColorTab.pal` palette contained only grayscale colors. The grayscale palette data is now **embedded inside** the compiled `.trk` files. Changing ColorTab.pal doesn't affect already-compiled tracks.

## Solution: Recompile Tracks

To fix the colors, you need to recompile the track files. Here's how:

## Method 1: Using HoverCad GUI (Easiest)

### Steps

1. **Open HoverCad**
   ```
   C:\originalhr\HoverRace\HoverCad\Release\HoverCad.exe
   ```

2. **Open a track source file**
   - File → Open
   - Navigate to: `C:\originalhr\HoverRace\NetTarget\mazes\`
   - Select `ClassicH.TR` (or other .TR file)
   - Click Open

3. **Compile the track**
   - Click the "Compile" button (or File → Compile)
   - Choose output filename and location: `C:\originalhr\HoverRace\Release\Tracks\mazes\ClassicH.trk`
   - Click Save
   - Wait for compilation to complete

4. **Repeat for all tracks**
   - ClassicH.TR
   - Steeplechase.TR
   - The Alley2.TR
   - Any others in `NetTarget\mazes\`

5. **Test the game**
   - Run `Game2.exe`
   - Load a track and verify colors are now visible instead of grayscale

### What's Happening Behind the Scenes

When you click "Compile" in HoverCad:

1. HoverCad reads the `.TR` binary file (which contains all track geometry)
2. HoverCad calls `GenerateOutputFile()` which creates a temporary `.txt` text file
3. The `.txt` file contains track data in a structured format (see TRACK_COMPILER_FORMAT.md)
4. HoverCad runs: `MazeCompiler.exe output.trk intermediate.txt`
5. MazeCompiler reads:
   - The `.txt` track definition
   - The `ColorTab.pal` palette file (from current working directory)
   - **Embeds the current palette into the compiled `.trk` file**

### Why This Works

The current `ColorTab.pal` file has **real colors** (not grayscale). When MazeCompiler runs during compilation, it will embed these real colors into the new `.trk` file.

## Method 2: Direct MazeCompiler Usage (For Automation)

If you have intermediate `.txt` files from HoverCad (saved as debug files), you can directly use MazeCompiler:

### Steps

1. **Get intermediate `.txt` file**
   - HoverCad saves debug copies in `%TEMP%\HC*.tmp.debug`
   - Or generate one by opening track in HoverCad and clicking Compile

2. **Copy ColorTab.pal to MazeCompiler directory**
   ```powershell
   Copy-Item "C:\originalhr\HoverRace\NetTarget\bitmaps\ColorTab.pal" `
             "C:\originalhr\HoverRace\Release\"
   ```

3. **Run MazeCompiler**
   ```powershell
   cd "C:\originalhr\HoverRace\Release"
   .\MazeCompiler.exe "Tracks\mazes\ClassicH.trk" "C:\path\to\intermediate.txt"
   ```

4. **Verify output**
   - Check that `.trk` file was created
   - Run game and test colors

### Key Points

- MazeCompiler MUST have `ColorTab.pal` in its working directory
- The command format is: `MazeCompiler <output.trk> <input.txt>`
- Output goes to the first argument path
- Input is read from the second argument path

## Method 3: Batch Automation (For Multiple Tracks)

If you want to automate this for multiple tracks:

### Using PowerShell Script

```powershell
# Run this script from the Release directory where MazeCompiler.exe and ColorTab.pal are located
cd "C:\originalhr\HoverRace\Release"

# Batch compile all .txt files found in temp
Get-ChildItem "$env:TEMP\HC*.tmp.debug" -ErrorAction SilentlyContinue | ForEach-Object {
    $outputName = $_.BaseName -replace '\.tmp\.debug', '.trk'
    $outputPath = "Tracks\mazes\$outputName"
    
    Write-Host "Compiling: $outputName"
    
    # Copy the debug file to a temp .txt file
    $txtFile = $_.FullName -replace '\.debug$', ''
    
    # Run MazeCompiler
    & .\MazeCompiler.exe $outputPath $txtFile
    
    if ($?) {
        Write-Host "  ✓ Success"
    } else {
        Write-Host "  ✗ Failed"
    }
}
```

## Verification

After recompiling, verify that colors are working:

### Check 1: File Size
- New `.trk` files should be roughly similar size to old ones
- If much smaller, compilation might have failed

### Check 2: Run Game
```
C:\originalhr\HoverRace\Release\Game2.exe
```
- Select a recompiled track
- Verify walls, floor, ceiling, and objects are now colored instead of grayscale

### Check 3: Examine Compilation Log
If MazeCompiler produces logs:
```
%TEMP%\MazeCompiler_Output.log
```

## Troubleshooting

### "Unable to find [HEADER] section"
- The intermediate file is not in the correct text format
- Try regenerating from HoverCad

### Colors still grayscale after recompiling
- Verify `ColorTab.pal` is in the MazeCompiler working directory
- Check that `ColorTab.pal` actually contains colors (not grayscale)
- Verify the new `.trk` file was actually created

### MazeCompiler.exe crashes or fails to run
- Ensure `ColorTab.pal` is present
- Try running from Command Prompt (not PowerShell) to see error messages
- Check that output path is writable

## Important Files

- **ColorTab.pal** (256-color palette, 768 bytes)
  - Location: `C:\originalhr\HoverRace\NetTarget\bitmaps\ColorTab.pal`
  - Must be present during MazeCompiler execution
  - Contains RGB color values that are embedded in `.trk` files

- **Track source files** (.TR format)
  - Location: `C:\originalhr\HoverRace\NetTarget\mazes\*.TR`
  - Binary format, read by HoverCad
  - Contains track geometry (rooms, walls, textures, objects)

- **Compiled track files** (.TRK format)
  - Location: `C:\originalhr\HoverRace\Release\Tracks\mazes\*.trk`
  - Binary format, read by Game2.exe
  - Contains embedded palette + track geometry

- **Intermediate format** (.TXT)
  - Temporary files in `%TEMP%\HC*.tmp`
  - Human-readable track definition
  - Created by HoverCad, consumed by MazeCompiler
  - See TRACK_COMPILER_FORMAT.md for detailed format specification

## Next Steps

1. Recompile tracks using HoverCad GUI (Method 1)
2. Test Game2.exe with recompiled tracks
3. Verify colors display correctly
4. If needed, set up batch automation for future track modifications

## References

- **TRACK_COMPILER_FORMAT.md** - Detailed specification of the intermediate text format
- **HoverCadDoc.cpp** - HoverCad source code showing GenerateOutputFile() implementation
- **MazeCompiler main.cpp** - MazeCompiler source code showing parsing logic
