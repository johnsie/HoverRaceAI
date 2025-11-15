# RaceServer Project

## Overview

The RaceServer is a centralized game server application for HoverRace that hosts multiple concurrent race sessions. It eliminates the firewall/NAT traversal issues of the peer-to-peer model by providing a dedicated server that players connect to directly.

## Architecture

### Project Structure

```
RaceServer/
├── RaceServer.cpp                 Main entry point
├── stdafx.h/cpp                   Precompiled headers
├── config.xml                     Runtime configuration
│
├── NetworkInterface/              Network communication layer
│   ├── ServerSocket.h/cpp         TCP/UDP server socket
│   ├── ClientConnection.h/cpp     Per-player connection state
│   └── MessageDispatcher.h/cpp    Message routing by type
│
├── GameSimulation/                Game logic and simulation
│   ├── GameSimulation.h/cpp       Core race simulation
│   └── GameState.h                Game state snapshot
│
├── RaceManager/                   Race session management
│   ├── RaceManager.h/cpp          Multi-race orchestrator
│   └── RaceSession.h/cpp          Single race instance
│
├── Config/                        Configuration management
│   ├── ServerConfig.h/cpp         Configuration loader
│   └── ServerConfig.cpp
│
├── Logging/                       Logging utilities
│   ├── ServerLogger.h/cpp         Logging system
│   └── ServerLogger.cpp
│
└── Tests/                         Unit tests
    ├── GameSimulation_Tests.cpp
    ├── MessageDispatcher_Tests.cpp
    └── RaceManager_Tests.cpp
```

### Component Responsibilities

1. **ServerSocket**: Manages TCP/UDP connections, accepts new clients, multiplexes socket I/O
2. **ClientConnection**: Tracks per-player state (name, lag, position, race assignment)
3. **MessageDispatcher**: Routes incoming network messages to appropriate handlers
4. **GameSimulation**: Core race logic - physics tick, player position updates, lap detection
5. **RaceSession**: Encapsulates a single race instance with its players
6. **RaceManager**: Orchestrates N concurrent race sessions
7. **ServerConfig**: Loads configuration from XML file
8. **ServerLogger**: Thread-safe logging to file

## Building

### Prerequisites

- Visual Studio 2015 or later
- Windows SDK
- HoverRace source code (engine/model/track/physics libraries)

### Build Steps

```bash
# Build RaceServer project
cd c:\originalhr2\HoverRaceAI\NetTarget\RaceServer
msbuild RaceServer.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=Win32

# Output: RaceServer.exe
```

## Running

### Basic Usage

```bash
RaceServer.exe <port> <logfile> [--config <config.xml>] [--max-races <n>]
```

### Examples

```bash
# Start on default port 9600
RaceServer.exe 9600 raceserver.log

# Start with custom configuration
RaceServer.exe 9600 raceserver.log --config config.xml --max-races 50

# Start on alternate port
RaceServer.exe 9700 raceserver_alt.log
```

### Output

```
HoverRace Centralized Race Server v1.0
=======================================

Server listening on port 9600
Press Ctrl+C to shutdown gracefully
```

## Configuration

Edit `config.xml` to customize server behavior:

```xml
<raceserver>
    <network>
        <port>9600</port>                    <!-- Listening port -->
        <max_connections>100</max_connections> <!-- Max concurrent TCP connections -->
    </network>
    
    <races>
        <max_concurrent>50</max_concurrent>  <!-- Max concurrent races -->
        <max_players_per_race>8</max_players_per_race>
        <idle_race_timeout_sec>300</idle_race_timeout_sec>  <!-- Clean up idle races -->
    </races>
    
    <logging>
        <level>INFO</level>                  <!-- DEBUG, INFO, WARN, ERROR -->
        <file>raceserver.log</file>
    </logging>
</raceserver>
```

## Protocol

RaceServer uses the same network message protocol as the original peer-to-peer system, ensuring minimal client changes.

### Message Flow

1. **Connection Handshake**
   - Client → Server: MRNM_SEND_KEYID (version check)
   - Client → Server: MRNM_CONN_NAME_GET_SET (exchange names)
   - Client → Server: MRNM_LAG_TEST (ping)
   - Server → Client: MRNM_LAG_ANSWER (pong)

2. **Race Setup**
   - Server → All Clients: MRNM_SET_TIME (sync game clock)
   - Server → All Clients: MRNM_CREATE_MAIN_ELEM (broadcast player joins)

3. **During Race**
   - Client → Server: MRNM_SET_MAIN_ELEM_STATE (player position/velocity)
   - Server → All Clients: MRNM_SET_MAIN_ELEM_STATE (broadcast positions)
   - Server → All Clients: MRNM_SET_PERM_ELEMENT_STATE (mines, power-ups)

4. **Race End**
   - Server → All Clients: MRNM_PLAYER_STATS (final results)

## Implementation Status

### Phase 1: Core Infrastructure ✓ Complete
- [x] ServerSocket with TCP/UDP support
- [x] ClientConnection state tracking
- [x] MessageDispatcher routing
- [x] Main event loop

### Phase 2: Game Engine ✓ Complete
- [x] GameSimulation extraction
- [x] GameState snapshot structure
- [x] RaceSession management
- [x] RaceManager orchestration

### Phase 3: InternetRoom Integration ⏳ In Progress
- [ ] Extend IRState::Game with server fields
- [ ] ADD_GAME_HOSTED HTTP command
- [ ] JOIN_GAME_HOSTED HTTP command
- [ ] START_GAME_HOSTED HTTP command

### Phase 4: Client Refactoring ⏳ Not Started
- [ ] Add CONNECTION_SERVER_HOSTED mode
- [ ] Dual-mode NetworkInterface
- [ ] Client connection logic
- [ ] Test client-server integration

### Phase 5: Configuration ✓ Mostly Complete
- [x] ServerConfig class
- [x] XML config file template
- [ ] Runtime configuration updates

### Phase 6: Testing ⏳ In Progress
- [ ] Unit tests (GameSimulation, RaceManager)
- [ ] Integration tests (client-server)
- [ ] Load tests (400 concurrent players)
- [ ] Performance profiling

## Performance Targets

- **Max Concurrent Races**: 50
- **Max Players Per Race**: 8
- **Max Total Players**: 400
- **Message Latency**: <100ms (authoritative position updates)
- **CPU Usage**: ~20% (Ryzen 7 / i7)
- **Memory Usage**: ~100 MB

## Debugging

### Enable Debug Logging

Edit `config.xml`:
```xml
<logging>
    <level>DEBUG</level>
</logging>
```

### Common Issues

**Connection refused**
- Check firewall: `netstat -an | grep 9600`
- Verify port is not in use: `netstat -ano | findstr :9600`

**Message parsing errors**
- Enable DEBUG logging to see raw message data
- Verify HoverRace client is sending correct message format

**Race not progressing**
- Check GameSimulation::Update() is being called
- Verify players are being added to race
- Check player position updates are arriving

## Next Steps

1. Implement remaining Phase 3 (InternetRoom integration)
2. Update client-side NetworkInterface for server mode
3. Comprehensive testing with real client connections
4. Performance optimization and load testing
5. Deployment to production infrastructure

## References

- Implementation Plan: `CENTRALIZED_SERVER_IMPLEMENTATION.md`
- Network Protocol: See codebase comments and existing NetInterface.h
- Configuration Guide: See config.xml comments
