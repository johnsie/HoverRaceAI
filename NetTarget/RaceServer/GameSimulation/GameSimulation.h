// GameSimulation.h : Core race simulation engine

#pragma once

#include "GameState.h"

class GameSimulation
{
public:
    GameSimulation();
    ~GameSimulation();

    // Initialize race with track, laps, rules
    BOOL Initialize(
        const char* trackName,
        int numLaps,
        BOOL allowWeapons,
        int maxPlayers = 8);

    // Add player to race (called when player connects)
    int AddPlayer(int clientId, const char* playerName);

    // Remove player (called when player disconnects)
    void RemovePlayer(int clientId);

    // Update game state (called each frame)
    void Update(float deltaTime);

    // Process player input/position update
    void ProcessPlayerUpdate(
        int clientId,
        const MR_Vector3& position,
        const MR_Vector3& velocity,
        const MR_Quaternion& rotation);

    // Get current game state to broadcast
    const GameState& GetCurrentState() const { return mState; }

    // Check if race is complete
    BOOL IsRaceComplete() const;

    // Start/stop race
    void StartRace();
    void StopRace();

    // Get player index by client ID
    int GetPlayerIndex(int clientId) const;

private:
    GameState mState;              // Current race state
    int mMaxPlayers;
    float mElapsedTime;
    BOOL mRaceStarted;
    BOOL mRaceComplete;

    // Helper methods
    void UpdatePlayerPositions(float deltaTime);
    void CheckLapCompletion();
    void CheckRaceCompletion();
};
