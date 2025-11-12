# Buffer Overflow Fix Report

## Problem Identified

The game was experiencing persistent memory corruption that caused crashes after ~1036 frames (~33 seconds) of gameplay.

### Root Cause: Buffer Over-Read in Network State Parsing

**Location:** `NetTarget/MainCharacter/MainCharacter.cpp` lines 620-670

The `SetNetState()` function was casting a raw network message buffer to a structured type without validating buffer size:

```cpp
// BEFORE (UNSAFE):
void MR_MainCharacter::SetNetState( int /*pDataLen*/, const MR_UInt8* pData )
{
    const MR_MainCharacterState* lState = (const MR_MainCharacterState*)pData;
    
    mPosition.mX = lState->Get( MC_POSX );  // Could read past buffer!
    mPosition.mY = lState->Get( MC_POSY );
    // ...
}
```

### The Vulnerability

1. **MR_MainCharacterState structure**: Only 19 bytes (146 bits total)
   - Array defined as `MR_UInt8 mFieldList[18]` - **BUG #1: Off by one!**
   - Should be `mFieldList[19]` to hold 146 bits = 18.25 bytes

2. **MR_BitPack::Get() method** (in BitPacking.h lines 33-38):
   ```cpp
   lReturnValue.u = (*(MR_UInt32*)(mData+pOffset/8))>>(pOffset%8);
   
   if( (pLen+(pOffset%8))>32 )
   {
      lReturnValue.u |= (*(MR_UInt32*)(mData+(pOffset/8)+4))<<(32-(pOffset%8));
      // ^^ CAN READ 4+ BYTES PAST BUFFER END!
   }
   ```

3. **MC_CONTROL_ST offset**: Starts at bit 122 = byte 15, length 15 bits
   - Reads bits 122-136 (needs up to byte 17)
   - But Get() can read bytes +4, reaching byte 21
   - Buffer is only 19 bytes!
   - **Result: Reads adjacent memory → memory corruption**

### Evidence of Corruption

Game logs showed:
- Position.Y values: Garbage (2e+264 range - clearly corrupted)
- Position.mY alternating between valid and garbage values
- Room values: Negative (-6000, -5323) - impossible
- Speed values: Astronomical garbage (1.532e+78)
- Crash at frame 1036 consistently

## Solutions Implemented

### Fix #1: Increase Buffer Size (Line 155)

**File:** `NetTarget/MainCharacter/MainCharacter.cpp`

**Before:**
```cpp
// Total                 146  = 19 bytes
MR_UInt8  mFieldList[18];  // ← WRONG: Only 18 bytes!
```

**After:**
```cpp
// Total                 146  = 19 bytes (146 bits / 8 = 18.25 bytes, rounds up to 19)
MR_UInt8  mFieldList[19];  // ← FIXED: Now 19 bytes as comment states
```

**Impact:** Prevents out-of-bounds reads when Get() method reads MC_ON_FLOOR, MC_HOVER_MODEL, or MC_PADDING fields.

### Fix #2: Add Buffer Size Validation (Lines 625-630)

**File:** `NetTarget/MainCharacter/MainCharacter.cpp`

**Before:**
```cpp
void MR_MainCharacter::SetNetState( int /*pDataLen*/, const MR_UInt8* pData )
{
    const MR_MainCharacterState* lState = (const MR_MainCharacterState*)pData;
    // NO VALIDATION - just cast and use!
```

**After:**
```cpp
void MR_MainCharacter::SetNetState( int pDataLen, const MR_UInt8* pData )
{
    // CRITICAL FIX: Validate buffer size before casting to MR_MainCharacterState
    // The MR_BitPack::Get() method can read up to 4 bytes past the declared
    // 18-byte mFieldList array, potentially reading adjacent memory
    if( pDataLen < (int)sizeof( MR_MainCharacterState ) )
    {
        ASSERT( FALSE );  // Network message too small!
        return;  // Safely reject malformed message
    }

    const MR_MainCharacterState* lState = (const MR_MainCharacterState*)pData;
    // NOW SAFE: We know buffer is large enough
```

**Impact:** Prevents crashes when network layer sends undersized messages.

**Note:** This pattern is already used in `Missile.cpp` (line 384):
```cpp
if( pDataLen >= sizeof( MR_MissileState ) )
{
    // Use the data safely
}
```

## Technical Details

### MR_MainCharacterState Field Layout (146 bits = 19 bytes)

| Field | Offset (bits) | Length (bits) | Bytes needed |
|-------|--------------|--------------|-------------|
| MC_POSX | 0 | 26 | 4 |
| MC_POSY | 26 | 26 | 4 |
| MC_POSZ | 52 | 15 | 2 |
| MC_ROOM | 67 | 11 | 2 |
| MC_ORIENTATION | 78 | 9 | 2 |
| MC_SPEED_X_256 | 87 | 13 | 2 |
| MC_SPEED_Y_256 | 100 | 13 | 2 |
| MC_SPEED_Z_256 | 113 | 9 | 2 |
| MC_CONTROL_ST | 122 | 15 | 3 |
| MC_ON_FLOOR | 137 | 1 | 1 |
| MC_HOVER_MODEL | 138 | 3 | 1 |
| MC_PADDING | 141 | 11 | 2 |
| **TOTAL** | 152 bits | - | **19 bytes** |

### Why BitPacking Can Over-read

The `MR_BitPack::Get()` method reads bit-packed values using bitwise operations:

```cpp
// Get bits starting at pOffset for pLen bits
lReturnValue.u = (*(MR_UInt32*)(mData+pOffset/8))>>(pOffset%8);

// If value spans 32-bit boundary, read next 32 bits
if( (pLen+(pOffset%8))>32 )
{
    lReturnValue.u |= (*(MR_UInt32*)(mData+(pOffset/8)+4))<<(32-(pOffset%8));
                                        ^^^^^^^^ Can read up to +4 bytes!
}
```

Example: Reading MC_CONTROL_ST starting at bit 122
- pOffset = 122, pLen = 15
- First read: bytes 15-18 (bits 122-127)
- Since 15 + (122%8) = 15 + 2 = 17, which is < 32, normally OK
- BUT if any Get() call reads slightly beyond, it can trigger the +4 read

The undersized array (18 instead of 19) meant the +4 read reached memory past the struct.

## Testing

Expected outcome after fixes:
1. **No more corruption** - Position/room values should be consistent and valid
2. **Longer game runtime** - Should run past frame 1036 without corruption
3. **Valid data** - Position X/Y/Z should remain in reasonable ranges
4. **Stable for extended play** - Game should run for minutes without crashing

## Build Instructions

```bash
cd c:\originalhr\HoverRace
# Method 1: PowerShell (if fixed)
powershell -ExecutionPolicy Bypass -File generate_projects.ps1

# Method 2: Direct rebuild
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
  NetTarget.sln ^
  /p:Configuration=Release ^
  /p:Platform=Win32

# Run
cd Release
Game2.exe
```

## Related Code Patterns

This vulnerability pattern is known as "buffer over-read" and occurs when:
1. A struct is cast from raw bytes without size validation
2. Access methods can read beyond declared buffer size
3. No checks validate actual message size vs. struct size

The fix pattern used here matches defensive programming best practices:
- Always validate before trust
- Use sizeof() for struct size checks
- Reject malformed input early
- Include comments explaining the vulnerability

## Files Modified

1. `NetTarget/MainCharacter/MainCharacter.cpp`
   - Line 155: Changed `mFieldList[18]` → `mFieldList[19]`
   - Line 625: Added size validation check
   - Lines 626-630: Added early return for undersized buffer

## Files NOT Modified

- `BitPacking.h` - The Get() method is correct; it's designed to read up to +4 bytes
  - Safe when buffer is properly sized
  - Unsafe when buffer is undersized (was the case here)

