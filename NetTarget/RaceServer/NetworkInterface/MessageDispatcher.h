// MessageDispatcher.h : Routes messages to appropriate handlers

#pragma once

class MR_RaceManager;

class MessageDispatcher
{
public:
    MessageDispatcher();

    // Dispatch message to handler based on type
    void Dispatch(
        int clientId,
        const void* pMessageData,
        int messageLength,
        MR_RaceManager* pRaceManager);

private:
    // Message handlers for each type
    void Handle_SendKeyId(int clientId, const void* pData, int dataLen);
    void Handle_ConnNameGetSet(int clientId, const void* pData, int dataLen);
    void Handle_ClientAddr(int clientId, const void* pData, int dataLen);
    void Handle_LagTest(int clientId, const void* pData, int dataLen);
    void Handle_CreateMainElem(int clientId, const void* pData, int dataLen);
    void Handle_SetMainElemState(int clientId, const void* pData, int dataLen);
    void Handle_ChatMessage(int clientId, const void* pData, int dataLen);
    void Handle_Ready(int clientId, const void* pData, int dataLen);
};
