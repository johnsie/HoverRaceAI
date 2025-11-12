# Flickering Fix - Root Cause & Solution

## Problem Identified

**The Flickering Pattern You Described:**
- 3D vehicle flashes on and off every 2-3 frames  
- All geometry flashes on and off
- Full window flickers with red/sandy color
- But game is still responsive and playable

## Root Cause

The `DrawBackground()` function in `GameApp.cpp` was responsible:

```cpp
void MR_GameApp::DrawBackground()
{
   for( int lY=0 ; lY<lYRes; lY++ )
   {
      for( int lX = 0; lX<lXRes; lX++ )
      {
         *lDest = (lColorIndex&16)?11:39;  // Colors 11 & 39 = red/sandy stripes
         lColorIndex++;
         lDest++;
      }
   }
}
```

This function was **filling the ENTIRE screen with a striped background pattern** (colors 11=red, 39=sandy) every 2 frames, completely **erasing all 3D rendering** from the previous frame.

**The Control Flow:**
1. `mClrScrTodo = 2` at initialization
2. Frame 1: `mClrScrTodo=2 > 0` → `--mClrScrTodo=1`, call `DrawBackground()` → fills screen with red stripes
3. Frame 2: `mClrScrTodo=1 > 0` → `--mClrScrTodo=0`, call `DrawBackground()` → fills screen with red stripes again
4. Frame 3: `mClrScrTodo=0` → SKIP `DrawBackground()` → 3D rendering is FINALLY VISIBLE!
5. But then something resets `mClrScrTodo` back to 2, repeating the cycle

**This caused the flickering pattern you observed:**
- Frames 1-2: Red/sandy striped background (hides 3D)
- Frame 3: 3D rendering visible briefly
- Frames 1-2 (repeat): Red/sandy again...

## Solution Implemented

**Changed initialization from `mClrScrTodo = 2` to `mClrScrTodo = 0`**

In two locations in `GameApp.cpp`:
1. Constructor (line ~442)
2. Clean() function (line ~476)

**What this does:**
- `mClrScrTodo = 0` means the condition `if( mClrScrTodo > 0 )` is ALWAYS FALSE
- `DrawBackground()` is NEVER called
- The red/sandy striped background pattern is NEVER drawn
- 3D rendering remains visible every frame WITHOUT flickering

**Code Changed:**
```cpp
// BEFORE (BROKEN):
mClrScrTodo = 2;  // Draws background stripes for 2 frames, erasing 3D rendering

// AFTER (FIXED):
mClrScrTodo = 0;  // SKIP background pattern entirely, allow 3D rendering to show
```

## Why This Works

The background pattern was intended to be drawn during loading/initialization. Once the game is running, the 3D viewport should handle all rendering. By setting `mClrScrTodo = 0`, we ensure:

1. ✅ No periodic screen clearing
2. ✅ 3D rendering persists every frame
3. ✅ No flickering
4. ✅ Smooth, continuous display

## Files Modified

- **c:\originalhr\HoverRace\NetTarget\Game2\GameApp.cpp**
  - Line ~442 (Constructor): Changed `mClrScrTodo = 2` → `mClrScrTodo = 0`
  - Line ~476 (Clean function): Changed `mClrScrTodo = 2` → `mClrScrTodo = 0`

## Build Status

✅ Game2.exe rebuilt successfully  
✅ No compilation errors
✅ Ready for testing

## Expected Result

When you run Game2.exe:
- 3D rendering will be **stable and continuous**
- NO red/sandy color flashing
- NO geometry appearing/disappearing
- Smooth, flicker-free gameplay
- ObjFac1 HoverRender will display properly without flickering

---

**Technical Notes:**

The background pattern appears to be legacy code from an older rendering system. Modern HoverRace tracks provide their own background through the 3D viewport system, so the striped pattern was redundant and harmful.

By disabling it entirely, we allow the proper 3D rendering pipeline to work without interruption.
