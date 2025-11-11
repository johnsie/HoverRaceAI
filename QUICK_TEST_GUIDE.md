# Quick Start: Test the Track Loading Fix

## ⚡ TL;DR

1. Open Command Prompt
2. Run: `cd C:\originalhr\HoverRace\Release && verify_fix.bat`
3. Look for: `[SUCCESS] Level geometry is being serialized`
4. If found: The fix is working! ✅
5. If not: MazeCompiler needs to be rebuilt

---

## 📋 Step-by-Step Guide

### Step 1: Open Command Prompt

Press `Windows + R`, type `cmd`, press Enter

### Step 2: Navigate to Release Directory

```batch
cd C:\originalhr\HoverRace\Release
```

### Step 3: Run Verification Script

```batch
verify_fix.bat
```

### Step 4: Read the Output

Look for these lines:

**If Fix is Working** ✅
```
[OK] MazeCompiler.exe found
[OK] Debug file found at %TEMP%\HCA972.tmp.debug
[OK] Output file created: Tracks\ClassicH_test.trk
[OK] New file is LARGER (contains level geometry)
[SUCCESS] Level geometry is being serialized
```

**If Fix is NOT Working** ❌
```
[ERROR] Found: "Skipping Serialize due to stability issues"
[FAILURE] Still using OLD buggy code - MazeCompiler was not rebuilt
```

### Step 5: What to Do Next

#### If Fix is Working ✅

1. Make a backup of the original:
   ```batch
   copy Tracks\ClassicH.trk Tracks\ClassicH_backup.trk
   copy Tracks\ClassicH_test.trk Tracks\ClassicH.trk
   ```

2. Do the same for all other tracks in Tracks directory

3. Test Game2.exe:
   ```batch
   Game2.exe
   ```

4. When game starts, select a track and try to race

5. The track should now display properly with:
   - Rooms and walls visible
   - Floor and ceiling rendered
   - Minimap displaying track layout
   - Full playable racing surface

#### If Fix is NOT Working ❌

1. MazeCompiler needs to be rebuilt

2. From the Release directory, run:
   ```batch
   "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
   ..\NetTarget\MazeCompiler\MazeCompiler.vcxproj ^
   /p:Configuration=Release /t:Rebuild
   ```

3. Then run `verify_fix.bat` again

---

## 🔍 Understanding the Output

### File Size Check

```
Input file: C:\Temp\HCA972.tmp.debug (7,383 bytes)
Output file: Tracks\ClassicH_test.trk (572 KB before fix)
```

**Expected after fix**: Output file should be **much larger** because it now contains level geometry

Example:
- Before fix: ~572 KB (header + background + map only)
- After fix: ~1-2 MB (header + background + map + level geometry)

### Key Log Messages

When MazeCompiler runs, it prints debug messages. Look for:

**Good Signs** ✅
```
DEBUG: Archive created, about to serialize level
DEBUG: lNewLevel is valid, calling Serialize...
DEBUG: Level serialization completed successfully
```

**Bad Signs** ❌
```
DEBUG: Skipping Serialize due to stability issues
DEBUG: [no serialization messages]
```

---

## 🎮 Testing in Game2.exe

After replacing the track file, launch Game2.exe and:

1. **Main Menu**: Should appear normally

2. **Track Selection**: Should show "ClassicH" as an available track

3. **Select Track**: Click on ClassicH

4. **Race Preview**: Should show a track minimap in the preview (Record 3)

5. **Start Race**: Should display:
   - Hovering over the racing surface (floor)
   - Walls around the track
   - Buildings and structures
   - Complete track layout
   - All 22 rooms interconnected properly

**If track displays**: ✅ Fix is successful!

**If track is still empty**: ❌ Something went wrong, check logs

---

## 📊 Troubleshooting

### Problem: "MazeCompiler.exe not found"

**Solution**: Make sure you're in `C:\originalhr\HoverRace\Release`

```batch
dir MazeCompiler.exe
```

Should show the file. If not, it wasn't built.

### Problem: "Debug file not found"

**Solution**: Need to create it in HoverCad

1. Open `C:\originalhr\HoverRace\HoverCad\Release\HoverCad.exe`
2. Open a track file (.TR)
3. Click "Compile" button
4. Select output location
5. Wait for compilation to complete

This creates `%TEMP%\HC*.tmp.debug` files

### Problem: "Skipping Serialize due to stability issues"

**Solution**: MazeCompiler was not rebuilt with the fix

1. Check the source code fix is in place:
   ```batch
   findstr /n "lNewLevel->Serialize" ..\NetTarget\MazeCompiler\main.cpp
   ```
   Should show line 423 with the serialization code

2. Rebuild MazeCompiler:
   ```batch
   cd C:\originalhr\HoverRace
   "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
   NetTarget\MazeCompiler\MazeCompiler.vcxproj /p:Configuration=Release /t:Rebuild
   ```

3. Run `verify_fix.bat` again

### Problem: Game2.exe crashes when loading track

**Possible causes**:
1. Track file is corrupted
2. Level geometry has errors
3. Out of memory

**Solution**:
1. Check `Release\Game2_TrackLoad.log` for error messages
2. Try a simpler track first
3. Increase virtual memory if needed

---

## 📝 Verification Checklist

- [ ] Opened Command Prompt successfully
- [ ] Changed to Release directory: `C:\originalhr\HoverRace\Release`
- [ ] Ran `verify_fix.bat`
- [ ] Saw `[OK]` messages for files found
- [ ] Saw `[SUCCESS]` message for serialization
- [ ] New test file is larger than original
- [ ] Backed up original track files
- [ ] Copied test files to replace originals
- [ ] Launched Game2.exe
- [ ] Selected a track from the menu
- [ ] Track displays with full geometry and playable surfaces
- [ ] Can move hovercraft around the track

---

## 📞 Support

If you encounter issues:

1. Check the verification script output in detail
2. Look at `mazecompiler_test.log` for compilation messages
3. Look at `Release\Game2_TrackLoad.log` for loading errors
4. Compare file sizes before and after
5. Check that MazeCompiler.exe is the newest version (built 11/11/2025)

---

## 🎉 Success Indicators

You'll know the fix is working when:

1. ✅ `verify_fix.bat` shows `[SUCCESS]`
2. ✅ New .trk file is significantly larger
3. ✅ Game2.exe loads the track without errors
4. ✅ Track displays complete geometry
5. ✅ Can race on the track and see all features
6. ✅ Minimap shows track layout
7. ✅ Multiple rooms/sections render properly

All 7 indicators = **Fix is fully working!** 🎮
