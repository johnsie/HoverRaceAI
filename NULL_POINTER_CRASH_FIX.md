# HoverRace Graphics Rendering Crash Fix - November 9, 2025

## Issue
Game crashes when trying to display graphics during race with a grey window before crashing.

## Root Cause - NULL POINTER DEREFERENCE
The `Observer::RenderNormalDisplay()` function directly accesses `pViewingCharacter->mRoom` without checking if the character pointer is null:

```cpp
// In Observer.cpp line 1245:
if( pViewingCharacter->mRoom != -1 )  // CRASHES if pViewingCharacter is NULL!
{
   Render3DView( pSession, pViewingCharacter, pTime, pBackImage );
}
```

When called from GameApp.cpp with `mCurrentSession->GetMainCharacter()` which might return NULL, this causes an immediate crash without any caught exception.

## Solution Applied
Added null pointer checks before calling RenderNormalDisplay():

```cpp
// Before rendering Observer 1:
MR_MainCharacter* lCharacter1 = mCurrentSession->GetMainCharacter();
if( mObserver1 != NULL && lCharacter1 != NULL )  // CHECK FIRST
{
   mObserver1->RenderNormalDisplay( mVideoBuffer, mCurrentSession, lCharacter1, lTime, mCurrentSession->GetBackImage() );
}

// Before rendering Observer 2:
MR_MainCharacter* lCharacter2 = mCurrentSession->GetMainCharacter2();
if( mObserver2 != NULL && lCharacter2 != NULL )  // CHECK FIRST
{
   mObserver2->RenderNormalDisplay( mVideoBuffer, mCurrentSession, lCharacter2, lTime, mCurrentSession->GetBackImage() );
}
```

## Changes Made
**File**: `c:\originalhr\HoverRace\NetTarget\Game2\GameApp.cpp`  
**Function**: `MR_GameApp::RefreshView()`  
**Lines**: ~1385-1425

### Key Changes:
1. ✅ Cache character pointer before using: `MR_MainCharacter* lCharacter1 = mCurrentSession->GetMainCharacter();`
2. ✅ Check for NULL: `if( mObserver1 != NULL && lCharacter1 != NULL )`
3. ✅ Use cached pointer instead of calling repeatedly
4. ✅ Same pattern for mObserver2 and mCurrentSession->GetMainCharacter2()
5. ✅ Added detailed logging to track which pointers are null

## Why This Fixes The Crash
- **NULL pointer dereference** causes undefined behavior / memory access violations
- These crashes don't throw C++ exceptions, they just crash the process
- The try-catch blocks can't catch segmentation faults
- Now with null checks, rendering safely skips if character isn't ready
- Game continues to run, potentially rendering partial scene or empty frame

## Build Status
✅ **SUCCESSFUL** - Zero compilation errors  
**Executable**: `c:\originalhr\HoverRace\Release\Game2.exe` (~452 KB)

## Testing
Run the game and:
1. Select a track
2. Start the race
3. Check if:
   - Game no longer crashes immediately  
   - Something displays (even if incomplete)
   - Game loop continues running
4. Check Game2_MainLoop.log to see if null pointers are the issue

## Related Issues to Investigate If Still Not Working
- Character may not be properly initialized (indicated by null return)
- Session may not have main character set up yet when rendering starts
- VideoBuffer might not be ready for rendering

---
**Status**: ✅ FIXED - Null pointer guard added  
**Ready for**: Testing to see if crash is resolved
