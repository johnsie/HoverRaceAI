# HoverRace Track Compilation Documentation Index

## Overview

This documentation package provides complete information about the HoverRace track compilation pipeline, including the grayscale palette issue analysis and solutions.

## 📋 Main Documents

### For Users (Want to Fix Grayscale Colors)
Start here if you want to recompile tracks and get colors back:

1. **[TRACK_RECOMPILATION_GUIDE.md](TRACK_RECOMPILATION_GUIDE.md)** ⭐ START HERE
   - Simple step-by-step instructions
   - Three different methods (GUI, Direct, Batch)
   - Troubleshooting section
   - Estimated time: 15 minutes per track using GUI method

### For Developers (Want to Understand How It Works)
Start here if you want technical understanding:

1. **[TRACK_COMPILATION_ANALYSIS.md](TRACK_COMPILATION_ANALYSIS.md)** ⭐ START HERE
   - Complete architecture overview
   - Source code analysis
   - Root cause analysis of grayscale issue
   - Recommendations for improvements

2. **[TRACK_COMPILER_FORMAT.md](TRACK_COMPILER_FORMAT.md)**
   - Complete format specification
   - Section-by-section breakdown
   - Reference guide for all fields

### For Automation/DevOps
Scripts and tools for batch processing:

1. **[compile_tracks.ps1](compile_tracks.ps1)** - PowerShell script
   - Automated batch recompilation
   - Color-coded output
   - Works on modern Windows systems

2. **[compile_tracks.bat](compile_tracks.bat)** - Batch script
   - Same functionality in cmd.exe format
   - Works on older Windows systems

3. **[recompile_tracks.ps1](recompile_tracks.ps1)** - Advanced script
   - Configuration management
   - Detailed reporting
   - Can be used as library/module

## 🎯 Quick Navigation

### "Why are the tracks grayscale?"
→ Read: [TRACK_RECOMPILATION_GUIDE.md](TRACK_RECOMPILATION_GUIDE.md) - "Problem Summary"
→ Then: [TRACK_COMPILATION_ANALYSIS.md](TRACK_COMPILATION_ANALYSIS.md) - "Current Grayscale Issue Analysis"

### "How do I fix it?"
→ Read: [TRACK_RECOMPILATION_GUIDE.md](TRACK_RECOMPILATION_GUIDE.md) - "Method 1: Using HoverCad GUI"
→ Time: ~15 minutes per track

### "Can I automate this?"
→ Read: [TRACK_RECOMPILATION_GUIDE.md](TRACK_RECOMPILATION_GUIDE.md) - "Method 3: Batch Automation"
→ Use: `compile_tracks.ps1` or `compile_tracks.bat`

### "What's the exact format?"
→ Read: [TRACK_COMPILER_FORMAT.md](TRACK_COMPILER_FORMAT.md)

### "How does the pipeline work?"
→ Read: [TRACK_COMPILATION_ANALYSIS.md](TRACK_COMPILATION_ANALYSIS.md) - "Architecture Overview"

### "Where can I find debug files?"
→ Read: [TRACK_COMPILER_FORMAT.md](TRACK_COMPILER_FORMAT.md) - "HoverCad Debug Files"
→ Location: `%TEMP%\HC*.tmp.debug`

## 📚 Document Cross-References

```
DOCUMENTATION_SUMMARY.md (this file)
├── TRACK_RECOMPILATION_GUIDE.md
│   ├── References to: TRACK_COMPILER_FORMAT.md
│   └── References to: TRACK_COMPILATION_ANALYSIS.md
│
├── TRACK_COMPILATION_ANALYSIS.md
│   ├── References to: TRACK_COMPILER_FORMAT.md
│   ├── References to: HoverCad/HoverCadDoc.cpp
│   └── References to: NetTarget/MazeCompiler/main.cpp
│
├── TRACK_COMPILER_FORMAT.md
│   ├── Example format from: %TEMP%\HC*.tmp.debug
│   └── References to: NetTarget/MazeCompiler/Parser.h
│
├── compile_tracks.ps1 (PowerShell automation)
├── compile_tracks.bat (Batch automation)
└── recompile_tracks.ps1 (Advanced management)
```

## 🔧 Tools Reference

### Compilation Tools

| Tool | Location | Purpose |
|------|----------|---------|
| HoverCad.exe | `HoverCad\Release\HoverCad.exe` | GUI track editor, generates intermediate format |
| MazeCompiler.exe | `Release\MazeCompiler.exe` | Compiles intermediate format to binary .trk |
| ColorTab.pal | `NetTarget\bitmaps\ColorTab.pal` | Color palette (256 colors) |

### Automation Scripts

| Script | Type | Platform | Use Case |
|--------|------|----------|----------|
| compile_tracks.ps1 | PowerShell | Windows 10+ | Batch automation with modern PowerShell |
| compile_tracks.bat | Batch | Windows 7+ | Batch automation with cmd.exe |
| recompile_tracks.ps1 | PowerShell | Windows 10+ | Advanced use, custom parameters |

### Files to Know About

| File Pattern | Location | Purpose |
|--------------|----------|---------|
| `*.TR` | `NetTarget\mazes\` | Source track files (binary, HoverCad format) |
| `*.trk` | `Release\Tracks\mazes\` | Compiled tracks (binary, game format) |
| `HC*.tmp` | `%TEMP%\` | Temporary intermediate files |
| `HC*.tmp.debug` | `%TEMP%\` | Debug copies (same as .tmp) |
| `ColorTab.pal` | `NetTarget\bitmaps\` | Palette data (256 colors × 3 bytes) |

## 📖 Reading Paths

### Path 1: Quick Fix (15 minutes)
1. Read: TRACK_RECOMPILATION_GUIDE.md (2 min)
2. Open: HoverCad and compile one track (10 min)
3. Test: Game2.exe to verify colors (3 min)
4. **Done!** Repeat for other tracks

### Path 2: Understanding (30 minutes)
1. Read: TRACK_COMPILATION_ANALYSIS.md - "Executive Summary" (5 min)
2. Read: TRACK_COMPILER_FORMAT.md - "File Structure" (10 min)
3. Read: TRACK_RECOMPILATION_GUIDE.md - "What's Happening" section (10 min)
4. Read: This index (5 min)
5. **Understanding achieved!**

### Path 3: Deep Dive (60 minutes)
1. Read: TRACK_COMPILATION_ANALYSIS.md - Entire document (20 min)
2. Read: TRACK_COMPILER_FORMAT.md - Entire document (15 min)
3. Read: TRACK_RECOMPILATION_GUIDE.md - Entire document (15 min)
4. Review: Source code references (10 min)
5. **Mastery achieved!**

### Path 4: Automation (45 minutes)
1. Read: TRACK_RECOMPILATION_GUIDE.md - "Method 3: Batch Automation" (5 min)
2. Study: compile_tracks.ps1 or compile_tracks.bat (10 min)
3. Generate: HoverCad debug files (15 min - see TRACK_RECOMPILATION_GUIDE.md)
4. Run: automation script (5 min)
5. Test: Verify colors in Game2.exe (10 min)
6. **Automation ready!**

## 🔍 Frequently Asked Questions

**Q: Why are tracks grayscale instead of colored?**
A: See TRACK_COMPILATION_ANALYSIS.md → "Current Grayscale Issue Analysis"

**Q: How do I get colors back?**
A: See TRACK_RECOMPILATION_GUIDE.md → "Method 1: Using HoverCad GUI" (easiest)

**Q: Can I automate this?**
A: Yes, see TRACK_RECOMPILATION_GUIDE.md → "Method 3: Batch Automation"

**Q: What's the intermediate file format?**
A: See TRACK_COMPILER_FORMAT.md (complete specification)

**Q: Where are the debug files?**
A: `%TEMP%\HC*.tmp.debug` (created when you click Compile in HoverCad)

**Q: Do I need to recompile all tracks?**
A: Yes, if you want all of them in color. Or just recompile the ones you want to use.

**Q: Will recompilation affect gameplay?**
A: No, only visual appearance. Gameplay remains identical.

**Q: Can I revert to grayscale?**
A: Yes, by recompiling with a grayscale ColorTab.pal, or restore old .trk files from git.

## ✅ Checklist for Complete Recompilation

- [ ] Read TRACK_RECOMPILATION_GUIDE.md
- [ ] Verify HoverCad.exe exists at: `HoverCad\Release\HoverCad.exe`
- [ ] Verify MazeCompiler.exe exists at: `Release\MazeCompiler.exe`
- [ ] Verify ColorTab.pal exists at: `NetTarget\bitmaps\ColorTab.pal`
- [ ] Open HoverCad
- [ ] [ ] Compile ClassicH.TR
- [ ] [ ] Compile Steeplechase.TR
- [ ] [ ] Compile The Alley2.TR
- [ ] [ ] Compile any other .TR files
- [ ] Run Game2.exe
- [ ] Test each track and verify colors display
- [ ] Commit recompiled .trk files to git
- [ ] Update build documentation if needed
- [ ] Mark complete ✓

## 📞 Troubleshooting

| Problem | Solution | Reference |
|---------|----------|-----------|
| "Unable to find [HEADER] section" | Wrong intermediate file format | TRACK_COMPILER_FORMAT.md |
| Colors still grayscale | Recompiled with old ColorTab.pal | TRACK_COMPILATION_ANALYSIS.md |
| MazeCompiler not found | Not in current directory | TRACK_RECOMPILATION_GUIDE.md |
| ColorTab.pal not found | Copy from NetTarget\bitmaps\ | compile_tracks scripts handle this |
| Script errors | Run from Release directory | See script comments |

## 🎓 Educational Value

This documentation package teaches:
- **Architecture**: How track compilation pipeline works
- **File Formats**: Intermediate text format specification
- **Root Cause Analysis**: Why grayscale issue exists
- **Problem Solving**: Multiple approaches to fix issue
- **Automation**: Batch processing and scripting
- **Debugging**: Understanding debug files and logging

## 📝 Version Information

- **Documentation Created**: 2025-11-11
- **HoverRace Version**: Latest (SDL2 graphics backend)
- **Compiler**: MazeCompiler (GrokkSoft 1996-1997)
- **Track Format**: Binary (.trk) with embedded palette
- **Palette Format**: JASC-PAL (256 colors, 768 bytes)

## 🔗 Related Resources

- HoverRace Official: http://www.hoverrace.com/
- GitHub (Official): https://github.com/HoverRace/HoverRace
- GitHub (AI Version): https://github.com/johnsie/HoverRaceAI

---

**Start with:** [TRACK_RECOMPILATION_GUIDE.md](TRACK_RECOMPILATION_GUIDE.md)

**Questions?** Check the "Frequently Asked Questions" section above or review the relevant documentation.

**Ready to fix colors?** See TRACK_RECOMPILATION_GUIDE.md → Method 1 (takes ~15 minutes per track)
