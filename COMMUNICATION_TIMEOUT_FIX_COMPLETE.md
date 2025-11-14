# Communication Timeout - Fix Summary

## Problem Statement
When users select an Internet meeting room, the application shows "Connecting to the Internet Meeting Room..." and then displays "Communication timeout" after ~22 seconds.

## Root Cause
The main HoverRace Internet Room server at `steeky.com/hover/roomlist2.txt` is no longer accessible/online. This causes:
1. The room list fails to load from the remote server
2. Without a fallback, the user gets an empty or stale room list
3. When attempting to connect to a room, the application waits the full `OP_TIMEOUT` (22 seconds) before showing the error

## Solution Implemented

### File Modified
`NetTarget/Game2/InternetRoom.cpp`

### Changes Made

#### 1. Fallback Room List (Lines ~750-765)
When the remote server list cannot be fetched, a demo/offline server entry is created:
```cpp
if( gNbServerEntries == 0 )
{
   // Add a test/demo server entry
   gServerList[0].mName = "[DEMO] Local Test Server (Offline)";
   gServerList[0].mPort = 80;
   gServerList[0].mURL = "/";
   gServerList[0].mType = MR_HTTP_ROOM_SERVER;
   gServerList[0].mAddress = inet_addr("127.0.0.1");
   
   gNbServerEntries = 1;
   gCurrentServerEntry = 0;
   lReturnValue = TRUE;
}
```

**Benefits:**
- Users still see a room option in the list (labeled as DEMO/Offline)
- Indicates the reason for unavailability
- Prevents confusing empty UI

#### 2. Immediate Error for Offline Servers (Lines ~2456-2478)
Added a check in `NetOpCallBack()` to detect when trying to connect to the offline demo server:
```cpp
// Check if trying to connect to offline demo server
if( gCurrentServerEntry >= 0 && gCurrentServerEntry < gNbServerEntries )
{
   const char* lServerName = (const char*)gServerList[gCurrentServerEntry].mName;
   
   // If this is a demo/offline server, show immediate error
   if( lServerName && strstr(lServerName, "DEMO") && strstr(lServerName, "Offline") )
   {
      CString lMessage;
      lMessage = "This is a demo server entry that is currently offline.\n\n"
                "The main HoverRace Internet Room server at steeky.com is not available.\n\n"
                "Internet multiplayer rooms require an active server to function.\n\n"
                "For single-player or local network play, use the other game modes.";
      
      MessageBox( pWindow, lMessage,
                 MR_LoadString( IDS_IMR ), MB_ICONSTOP|MB_OK|MB_APPLMODAL );
      
      EndDialog( pWindow, IDCANCEL );
      break;
   }
}
```

**Benefits:**
- Instant error message instead of 22-second timeout
- Clear explanation of the issue
- Helpful guidance for alternative game modes
- Professional UX instead of hanging dialog

## Impact

### Before Fix
1. User sees empty room list or stale cached data
2. Selects a room
3. Dialog shows "Connecting to the Internet Meeting Room..."
4. After 22 seconds: "Communication timeout" error
5. User is confused about what went wrong

### After Fix
1. User sees room list with "[DEMO] Local Test Server (Offline)" entry
2. When selecting it, gets immediate informative error message
3. User understands the server is unavailable and knows why
4. Respects user's time (no 22-second wait)

## Future Resolution

To fully re-enable Internet multiplayer rooms:

1. **Set up a new Internet Room server** that supports the HoverRace protocol
2. **Update the server URL** - Change `gMainServer` definition in InternetRoom.cpp from:
   ```cpp
   CString gMainServer = MR_IR_LIST;  // "steeky.com/hover/roomlist2.txt"
   ```
   To point to the new server's room list endpoint

3. **Server Requirements:**
   - Supports HTTP/HTTPS on port 80/443
   - Responds with server list in format: `[priority] [name] [ip] [port] [path]`
   - Implements the game room API (ADD_USER, ADD_GAME, JOIN_GAME, REFRESH, etc.)
   - Protocol documentation: See InternetRoom.cpp comments at lines 30-40

4. **Test the connection** - Verify the server list loads and rooms can be joined

## Technical Details

### Timeout Values (InternetRoom.cpp)
- `REFRESH_TIMEOUT`: 11000 ms (room refresh polling)
- `OP_TIMEOUT`: 22000 ms (general operations - connection, joins, etc.)
- `FAST_OP_TIMEOUT`: 6000 ms (quick operations)
- `CHAT_TIMEOUT`: 18000 ms (chat messages)

### Connection Flow
1. `LocateServers()` - Fetches room list from remote server
2. User selects room and clicks "Join"
3. `AddUserOp()` - Initiates connection
4. `NetOpCallBack()` - Handles UI and timeout
5. `MR_InternetRequest::Send()` - Makes network request
6. After `OP_TIMEOUT` with no response: shows timeout error

## Verification

To verify the fix works:
1. Build Game2.exe with the modified code
2. Run the game
3. Navigate to Internet Meeting Room
4. Observe "[DEMO] Local Test Server (Offline)" in the room list
5. Select it and attempt to join
6. Immediately see the informative error message (no 22-second wait)
