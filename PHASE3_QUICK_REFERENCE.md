# Phase 3 Completion - Quick Reference

## What Was Built

Three new HTTP commands added to InternetRoom for server-hosted races:

### 1. ADD_GAME_HOSTED
```
URL?=ADD_GAME_HOSTED USER_ID GAME_NAME TRACK_NAME NBLAP WEAPON SERVER_RACE_ID
Example: URL?=ADD_GAME_HOSTED 0-12345 "MyRace" "Cylinder" 5 1 42

Response on Success:
SUCCESS
GAME_ID 3-1000
SERVER_ADDR 127.0.0.1:9600

Response on Error:
ERROR 401 (user not online)
ERROR 402 (no game slots available)
```

### 2. JOIN_GAME_HOSTED
```
URL?=JOIN_GAME_HOSTED GAME_ID USER_ID SERVER_RACE_ID
Example: URL?=JOIN_GAME_HOSTED 3-1000 1-54321 42

Response on Success:
SUCCESS
SERVER_ADDR 127.0.0.1:9600

Response on Error:
ERROR 501 (user not online)
ERROR 502 (game not found)
ERROR 503 (game full)
```

### 3. START_GAME_HOSTED
```
URL?=START_GAME_HOSTED GAME_ID USER_ID SERVER_RACE_ID
Example: URL?=START_GAME_HOSTED 3-1000 0-12345 42

Response on Success:
SUCCESS

Response on Error:
ERROR 901 (user not online)
ERROR 902 (game not found)
ERROR 903 (not game owner)
```

---

## Game Structure Extension

The `IRState::Game` class now includes:

```cpp
BOOL      mServerHosted;       // TRUE = central server, FALSE = P2P
char      mServerAddr[16];     // e.g., "127.0.0.1"
unsigned  mServerPort;         // e.g., 9600
int       mServerRaceId;       // Opaque race ID from RaceServer
```

---

## How Client (Phase 4) Will Use This

### Step 1: Create Race (Server-Hosted Option)
```cpp
// User chooses "Use central server" in UI
POST /InternetRoom?=ADD_GAME_HOSTED 0-12345 "MyRace" "Cylinder" 5 1 42
// Server responds with game_id and server_addr:port
```

### Step 2: Other Players Join
```cpp
// Players browse available races
// They see mServerHosted=TRUE flag in game list
// They POST: JOIN_GAME_HOSTED game_id user_id server_race_id
// Server responds with connection details
```

### Step 3: Race Owner Starts
```cpp
// Race owner clicks "Start Race"
// POST: START_GAME_HOSTED game_id user_id server_race_id
// Server marks game as started
```

### Step 4: All Clients Connect to RaceServer
```cpp
// All clients receive: SERVER_ADDR 127.0.0.1:9600
// They TCP connect to RaceServer
// RaceServer begins sending game state updates
// Game proceeds with centralized server authority
```

---

## Files to Update in Phase 4

### Primary: NetTarget/NetInterface/NetInterface.h/cpp
- Add `enum ConnectionMode { CONNECTION_PEER_TO_PEER, CONNECTION_SERVER_HOSTED }`
- Add `SetConnectionMode(ConnectionMode mode)` method
- Update Connect() to route to RaceServer if SERVER_HOSTED mode
- Ensure message send/receive works for both modes

### Secondary: NetTarget/Game2/Game2.cpp (Main game loop)
- Detect mServerHosted flag from InternetRoom query
- Pass server address to NetInterface
- Use CONNECTION_SERVER_HOSTED mode if mServerHosted=TRUE

### Tertiary: UI dialogs
- Add option: "Host on my computer" vs "Use central server"
- Display server address when joining

---

## Configuration (TODO)

Current hardcoded values in AddGameHosted():
```cpp
strcpy( mGame[ lCounter ].mServerAddr, "127.0.0.1" );  // TODO: From config
mGame[ lCounter ].mServerPort = 9600;                   // TODO: From config
```

Should read from `InternetRoom/config.xml`:
```xml
<server-hosted>
    <default-address>central.hoverrace.com</default-address>
    <default-port>9600</default-port>
</server-hosted>
```

Or from environment:
```cpp
const char* addr = getenv("RACESERVER_ADDR");
const char* port = getenv("RACESERVER_PORT");
```

---

## Testing Checklist

### Unit Tests
- [ ] AddGameHosted() creates game with correct fields
- [ ] JoinGameHosted() adds players to game
- [ ] StartGameHosted() marks game as started
- [ ] Error codes returned correctly for invalid inputs

### Integration Tests
- [ ] InternetRoom HTTP endpoint accepts commands
- [ ] RaceServer receives game creation requests
- [ ] Multiple clients can join server-hosted race
- [ ] Race starts successfully after START_GAME_HOSTED

### E2E Tests
- [ ] Client creates server-hosted race via HTTP
- [ ] Other clients join via JOIN_GAME_HOSTED
- [ ] All clients receive server connection details
- [ ] Clients connect to RaceServer and race executes
- [ ] Race concludes and results are recorded

### Regression Tests
- [ ] P2P races still work (backward compatibility)
- [ ] Legacy clients can still play
- [ ] No performance regression in HTTP handling

---

## Performance Notes

### Memory
- 24 bytes per game slot × ~300 slots = 7.2 KB additional
- Negligible on modern systems

### CPU
- HTTP parsing unchanged (same format as other commands)
- No additional loops or complex algorithms
- Game lookup still O(1)

### Network
- RaceServer handles all message routing (not InternetRoom)
- Actual game traffic goes directly to RaceServer
- InternetRoom just tracks game metadata

---

## Security Considerations

### Race ID Validation
The `mServerRaceId` field prevents:
- Players joining wrong race by ID collision
- Replaying old START_GAME_HOSTED commands
- Cross-server race injection (future multi-DC)

### Player Validation
- Player must be online when joining
- Only game owner can start race
- Game capacity limits enforced

### Future Hardening
- Rate limiting on game creation
- Spam/DOS protection on JOIN
- Player reputation/bans system

---

## Deployment Checklist

- [ ] Build InternetRoom with Phase 3 changes
- [ ] Build RaceServer (already exists from Phase 1-2)
- [ ] Configure RaceServer address in config
- [ ] Test both P2P and server-hosted races
- [ ] Gradual rollout to testing players
- [ ] Monitor InternetRoom logs for errors
- [ ] Collect feedback from early adopters

---

## Branch Info

- **Branch**: CentralisedRaceServer
- **Base**: Original HoverRace source
- **Phase**: 3 of 7 COMPLETE

### Next Phases
- **Phase 4**: Client Dual-Mode Networking (2-3 weeks)
- **Phase 5**: Player Statistics Database (2 weeks)
- **Phase 6**: Comprehensive Testing Suite (2-3 weeks)
- **Phase 7**: Production Deployment (1-2 weeks)

---

Generated: November 14, 2025
