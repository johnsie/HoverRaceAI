// RaceManager.h : Manages multiple concurrent race sessions

#pragma once

#include "RaceSession.h"

class MR_RaceManager
{
public:
    MR_RaceManager();
    ~MR_RaceManager();

    // Initialize with max concurrent races
    BOOL Initialize(int maxConcurrentRaces = 50);

    // Create new race
    int CreateRace(
        const char* raceName,
        const char* trackName,
        int numLaps,
        BOOL allowWeapons,
        int creatorClientId);

    // Player joins existing race
    BOOL JoinRace(int raceId, int clientId, const char* playerName);

    // Player leaves race
    void LeaveRace(int raceId, int clientId);

    // Start a race
    BOOL StartRace(int raceId);

    // Update all races
    void UpdateAllRaces(float deltaTime);

    // Get race by ID
    RaceSession* GetRace(int raceId);

    // Get statistics
    int GetActiveRaceCount() const;
    int GetTotalPlayerCount() const;

    // Shutdown
    void Shutdown();

private:
    std::map<int, RaceSession*> mRaces;
    int mNextRaceId;
    int mMaxConcurrentRaces;

    // Helper
    void CleanupEmptyRaces();
};
