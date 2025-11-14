# Communication Timeout Fix - Internet Meeting Room

## Problem
When selecting an Internet meeting room, the application shows:
1. "Connecting to the Internet Meeting Room..."
2. Then displays "Communication timeout"

## Root Cause Analysis

The issue occurs in `NetTarget/Game2/InternetRoom.cpp` and is caused by two factors:

### Factor 1: Unavailable Server List
- The application attempts to fetch the room list from `steeky.com/hover/roomlist2.txt` (line 57)
- This server is no longer online/accessible
- When the fetch fails, `gNbServerEntries` remains 0
- Previously, no error was shown to the user - the app would proceed with an empty server list

### Factor 2: Attempting Connection to Non-existent Server
- When a room is selected and the user clicks to join, `AddUserOp()` is called (line 772+)
- This calls `mOpRequest.Send()` with the server address (line 2456)
- Without any servers in the list, this connection attempt times out
- The timeout is defined as `OP_TIMEOUT` = 22000 ms (22 seconds) (line 42)

## Code Flow
1. User selects room from list
2. `LocateServers()` called → tries to fetch server list from steeky.com
3. Fetch fails → `gNbServerEntries = 0`
4. Room list dialog shows empty or old cached data
5. User selects a room and clicks join
6. `AddUserOp()` attempts connection
7. `MR_InternetRequest::Send()` initiates network request
8. After 22 seconds with no response → timeout error shown

## Solution Implemented

Modified `LocateServers()` function (lines 700-780) to:
1. Still attempt to fetch from remote server
2. If fetch fails or returns no servers, display a user-friendly error message instead of trying to connect to a non-existent server
3. Error message explains that the server is unavailable and internet rooms require an active server

### Key Changes in InternetRoom.cpp
```cpp
// If fetch failed or returned no servers, show error message
if( gNbServerEntries == 0 )
{
   OutputDebugString("LocateServers: Remote server unavailable, unable to get server list");
   
   if( pParentWindow != NULL )
   {
      CString lMessage;
      lMessage.LoadString( IDS_COMM_ERROR );
      
      if( lMessage.IsEmpty() )
      {
         lMessage = "Unable to connect to Internet Meeting Room server.\n\n"
                   "The server at steeky.com is currently unavailable.\n\n"
                   "Internet multiplayer rooms require an active server to function.";
      }
      
      MessageBox( pParentWindow, lMessage, "HoverRace Internet Room", 
                 MB_ICONERROR | MB_OK | MB_APPLMODAL );
   }
   
   lReturnValue = FALSE;
}
```

## Alternative Solutions Considered

1. **Fallback to localhost (127.0.0.1)**: Rejected because there's no actual server running locally
2. **Mock server responses**: Too complex and would require significant changes
3. **Disable Internet Rooms**: Would lose functionality but could be an option if no server is available
4. **Show warning but allow attempt**: Current approach - show clear error before attempting connection

## Files Modified
- `NetTarget/Game2/InternetRoom.cpp` - Added error handling in `LocateServers()` function

## Testing Required
1. Run Game2.exe
2. Navigate to Internet Meeting Room
3. Observe that instead of a timeout after 22 seconds, user gets immediate error message
4. Error should clearly indicate the server is unavailable

## Future Work
To fully enable Internet rooms:
1. Set up a new meeting room server
2. Update the `gMainServer` URL to point to the new server
3. Ensure the server responds with properly formatted room list data
4. Server should support HTTP/HTTPS communication on port 80/443
