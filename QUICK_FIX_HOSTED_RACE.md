# Quick Fix Guide: Hosted Race Connection Error

## Error Message
```
An error occurred while starting the Internet session.
```

## What This Means
You're trying to join a race hosted on the centralized RaceServer, but the client couldn't connect properly.

## Quick Solutions (Try In Order)

### 1. **Verify RaceServer is Running**
```powershell
cd NetTarget\RaceServer\build\Release
.\RaceServer.exe
```

If it crashes or won't start, the RaceServer has an issue. Check `RaceServer.log` for details.

### 2. **Check Game List**
- In the Internet Room, verify the game shows as "Hosted" or has a special icon
- If the game is NOT showing as hosted, the lobby server isn't sending the RaceServer information

### 3. **For Remote RaceServer**
If your RaceServer is running on a different machine:

**Windows (Command Prompt)**:
```batch
set RACESERVER_ADDR=your-server-address
set RACESERVER_PORT=9600
```

**PowerShell**:
```powershell
$env:RACESERVER_ADDR="your-server-address"
$env:RACESERVER_PORT="9600"
```

Then start InternetRoom.exe and Game2.exe in the same terminal session.

### 4. **Check Debug Log**
After the error occurs, look for:
```
NewInternetSession.log
```

This file contains detailed error information that can help identify the exact problem.

### 5. **Check Network Connectivity**
```powershell
# Test if RaceServer is reachable
Test-NetConnection -ComputerName your-server-address -Port 9600 -InformationLevel Detailed
```

If it fails:
- RaceServer is down
- Firewall is blocking port 9600
- Wrong address/hostname

## If Still Not Working

1. **Check InternetRoom.log** (in InternetRoom working directory)
   - Look for "ADD_GAME_HOSTED" entries
   - Look for "SERVER_ADDR" output

2. **Check RaceServer output** (console window)
   - Look for connection attempts
   - Look for any error messages

3. **Restart Everything**:
   - Close all Game2.exe instances
   - Close InternetRoom.exe
   - Restart RaceServer.exe
   - Open InternetRoom.exe
   - Create/join hosted race

## Common Issues

| Issue | Solution |
|-------|----------|
| "Server address or port is missing" | Lobby server not configured properly, or ADD_GAME_HOSTED not sent |
| RaceServer won't start | Missing dependencies, port 9600 in use, or corrupted executable |
| Lobby server not showing games | Check InternetRoom.exe is running and HTTP port is accessible |
| Connection timeout | RaceServer unreachable, firewall blocking, or address wrong |
| P2P games work but hosted don't | RaceServer not running or not on port 9600 |

## Support Information

Provide these files when reporting issues:
- `NewInternetSession.log` (from Game2 working directory)
- `Game2_TrackLoad.log` (from Game2 working directory)  
- RaceServer console output (screenshot or log file)
- InternetRoom.log (if available)

Include:
- Your RACESERVER_ADDR setting (if not default)
- Your RACESERVER_PORT setting (if not default)
- Network setup (local vs remote RaceServer)
- Whether P2P games work (helps isolate issue)
