// RaceManager.cpp : Race manager implementation

#include "stdafx.h"
#include "RaceManager.h"
#include "ServerLogger.h"

extern MR_ServerLogger g_Logger;

MR_RaceManager::MR_RaceManager()
    : mNextRaceId(1),
      mMaxConcurrentRaces(50)
{
}

MR_RaceManager::~MR_RaceManager()
{
    Shutdown();
}

BOOL MR_RaceManager::Initialize(int maxConcurrentRaces)
{
    mMaxConcurrentRaces = maxConcurrentRaces;
    g_Logger.Log(MR_LOG_INFO, "Race manager initialized for %d max concurrent races", maxConcurrentRaces);
    return TRUE;
}

int MR_RaceManager::CreateRace(
    const char* raceName,
    const char* trackName,
    int numLaps,
    BOOL allowWeapons,
    int creatorClientId)
{
    if ((int)mRaces.size() >= mMaxConcurrentRaces) {
        g_Logger.Log(MR_LOG_WARN, "Cannot create race: maximum concurrent races (%d) reached",
                    mMaxConcurrentRaces);
        return -1;
    }

    int raceId = mNextRaceId++;
    RaceSession* pRace = new RaceSession();

    if (!pRace->Initialize(raceId, raceName, trackName, numLaps, allowWeapons)) {
        delete pRace;
        return -1;
    }

    mRaces[raceId] = pRace;

    g_Logger.Log(MR_LOG_INFO, "Race created: id=%d, name=%s, track=%s, creator=%d",
                raceId, raceName, trackName, creatorClientId);
    return raceId;
}

BOOL MR_RaceManager::JoinRace(int raceId, int clientId, const char* playerName)
{
    RaceSession* pRace = GetRace(raceId);
    if (!pRace) {
        g_Logger.Log(MR_LOG_WARN, "Cannot join: race %d not found", raceId);
        return FALSE;
    }

    if (pRace->GetActivePlayerCount() >= 8) {
        g_Logger.Log(MR_LOG_WARN, "Cannot join race %d: race full", raceId);
        return FALSE;
    }

    int playerIndex = pRace->AddPlayer(clientId, playerName);
    return (playerIndex >= 0);
}

void MR_RaceManager::LeaveRace(int raceId, int clientId)
{
    RaceSession* pRace = GetRace(raceId);
    if (pRace) {
        pRace->RemovePlayer(clientId);
    }
}

BOOL MR_RaceManager::StartRace(int raceId)
{
    RaceSession* pRace = GetRace(raceId);
    if (!pRace) {
        g_Logger.Log(MR_LOG_WARN, "Cannot start: race %d not found", raceId);
        return FALSE;
    }

    pRace->StartRace();
    return TRUE;
}

void MR_RaceManager::UpdateAllRaces(float deltaTime)
{
    for (auto& pair : mRaces) {
        RaceSession* pRace = pair.second;
        if (pRace) {
            pRace->Update(deltaTime);
        }
    }

    // Periodically clean up empty races
    static float cleanupTimer = 0.0f;
    cleanupTimer += deltaTime;
    if (cleanupTimer >= 5.0f) {  // Clean up every 5 seconds
        cleanupTimer = 0.0f;
        CleanupEmptyRaces();
    }
}

RaceSession* MR_RaceManager::GetRace(int raceId)
{
    auto it = mRaces.find(raceId);
    if (it != mRaces.end()) {
        return it->second;
    }
    return NULL;
}

int MR_RaceManager::GetActiveRaceCount() const
{
    return (int)mRaces.size();
}

int MR_RaceManager::GetTotalPlayerCount() const
{
    int total = 0;
    for (auto& pair : mRaces) {
        const RaceSession* pRace = pair.second;
        if (pRace) {
            total += pRace->GetActivePlayerCount();
        }
    }
    return total;
}

void MR_RaceManager::CleanupEmptyRaces()
{
    time_t currentTime = time(NULL);
    std::vector<int> racesToDelete;

    for (auto& pair : mRaces) {
        int raceId = pair.first;
        RaceSession* pRace = pair.second;
        if (pRace && pRace->ShouldCleanup(currentTime, 300)) {
            racesToDelete.push_back(raceId);
        }
    }

    for (int raceId : racesToDelete) {
        auto it = mRaces.find(raceId);
        if (it != mRaces.end()) {
            delete it->second;
            mRaces.erase(it);
            g_Logger.Log(MR_LOG_DEBUG, "Race %d cleaned up and removed", raceId);
        }
    }
}

void MR_RaceManager::Shutdown()
{
    for (auto& pair : mRaces) {
        delete pair.second;
    }
    mRaces.clear();
    g_Logger.Log(MR_LOG_INFO, "Race manager shutdown complete");
}
