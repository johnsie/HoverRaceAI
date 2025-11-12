# Quick Reference: Buffer Overflow Fix

## What Was The Problem?

Game crashed after ~1036 frames (33 seconds) due to memory corruption from reading past buffer boundary.

## Root Cause

1. **MR_MainCharacterState** had **18-byte array** but needed **19 bytes** (146 bits)
2. **SetNetState()** didn't validate buffer size before using data
3. **MR_BitPack::Get()** reads up to 4 extra bytes when extracting bit-packed values
4. Result: **Out-of-bounds read → memory corruption**

## The Two Fixes

### Fix #1: Line 155 (Struct Definition)
```cpp
// BEFORE (WRONG):
MR_UInt8  mFieldList[18];

// AFTER (CORRECT):
MR_UInt8  mFieldList[19];
```

**Why:** 146 bits = 18.25 bytes → rounds up to 19. Comment already said 19 bytes!

### Fix #2: Lines 620-630 (Buffer Validation)
```cpp
// BEFORE (UNSAFE):
void MR_MainCharacter::SetNetState( int /*pDataLen*/, const MR_UInt8* pData )
{
    const MR_MainCharacterState* lState = (const MR_MainCharacterState*)pData;
    // NO VALIDATION!
}

// AFTER (SAFE):
void MR_MainCharacter::SetNetState( int pDataLen, const MR_UInt8* pData )
{
    if( pDataLen < (int)sizeof( MR_MainCharacterState ) )
    {
        ASSERT( FALSE );
        return;
    }
    const MR_MainCharacterState* lState = (const MR_MainCharacterState*)pData;
    // NOW SAFE
}
```

**Why:** Prevents casting undersized buffers → prevents over-reads

## How to Build

```bash
cd c:\originalhr\HoverRace

# Option 1: Use Visual Studio
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
  NetTarget.sln ^
  /p:Configuration=Release ^
  /p:Platform=Win32

# Option 2: Use Visual Studio GUI
# File → Open → NetTarget.sln
# Build → Rebuild Solution (Release/Win32)
```

## How to Test

```bash
cd Release
Game2.exe
```

**Expected:**
- Game runs smoothly past frame 1036
- Position/room values are valid and consistent
- No crashes due to memory corruption
- Can play for extended periods

## Files Changed

- `NetTarget/MainCharacter/MainCharacter.cpp` (Lines 155, 625-630)

## Files NOT Changed

- `BitPacking.h` - The Get() implementation is correct, just needed proper buffer sizes

## Verification

The fixes are **minimal and focused**:
- ✅ Just 1 byte added to array
- ✅ Just validation check added
- ✅ No other code modified
- ✅ No API changes
- ✅ Backwards compatible

## Documentation

- `BUFFER_OVERFLOW_ROOT_CAUSE_AND_FIX.md` - Complete technical analysis
- `BUFFER_OVERFLOW_FIX.md` - Detailed explanation with code examples

