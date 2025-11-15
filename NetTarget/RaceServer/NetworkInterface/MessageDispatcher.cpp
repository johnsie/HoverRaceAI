// MessageDispatcher.cpp : Message dispatch implementation

#include "stdafx.h"
#include "MessageDispatcher.h"
#include "ServerLogger.h"

extern MR_ServerLogger g_Logger;

MessageDispatcher::MessageDispatcher()
{
}

void MessageDispatcher::Dispatch(
    int clientId,
    const void* pMessageData,
    int messageLength,
    MR_RaceManager* pRaceManager)
{
    if (!pMessageData || messageLength < 3) {
        return;  // Invalid message
    }

    // Parse message header
    const unsigned char* pData = (const unsigned char*)pMessageData;
    int messageType = pData[2] & 0x3F;  // Lower 6 bits = message type

    switch (messageType) {
        case 9:   // MRNM_SEND_KEYID
            Handle_SendKeyId(clientId, pData, messageLength);
            break;
        case 43:  // MRNM_CONN_NAME_GET_SET
            Handle_ConnNameGetSet(clientId, pData, messageLength);
            break;
        case 45:  // MRNM_CLIENT_ADDR_REQ
            Handle_ClientAddr(clientId, pData, messageLength);
            break;
        case 47:  // MRNM_LAG_TEST
            Handle_LagTest(clientId, pData, messageLength);
            break;
        case 2:   // MRNM_CREATE_MAIN_ELEM
            Handle_CreateMainElem(clientId, pData, messageLength);
            break;
        case 3:   // MRNM_SET_MAIN_ELEM_STATE
            Handle_SetMainElemState(clientId, pData, messageLength);
            break;
        case 6:   // MRNM_CHAT_MESSAGE
            Handle_ChatMessage(clientId, pData, messageLength);
            break;
        case 51:  // MRNM_READY
            Handle_Ready(clientId, pData, messageLength);
            break;
        default:
            g_Logger.Log(MR_LOG_DEBUG, "Unknown message type: %d from client %d", messageType, clientId);
    }
}

void MessageDispatcher::Handle_SendKeyId(int clientId, const void* pData, int dataLen)
{
    g_Logger.Log(MR_LOG_DEBUG, "Client %d: MRNM_SEND_KEYID", clientId);
    // TODO: Parse version/key information
}

void MessageDispatcher::Handle_ConnNameGetSet(int clientId, const void* pData, int dataLen)
{
    g_Logger.Log(MR_LOG_DEBUG, "Client %d: MRNM_CONN_NAME_GET_SET", clientId);
    // TODO: Exchange player names
}

void MessageDispatcher::Handle_ClientAddr(int clientId, const void* pData, int dataLen)
{
    g_Logger.Log(MR_LOG_DEBUG, "Client %d: MRNM_CLIENT_ADDR_REQ", clientId);
    // TODO: Send client address back
}

void MessageDispatcher::Handle_LagTest(int clientId, const void* pData, int dataLen)
{
    g_Logger.Log(MR_LOG_DEBUG, "Client %d: MRNM_LAG_TEST", clientId);
    // TODO: Send LAG_ANSWER response
}

void MessageDispatcher::Handle_CreateMainElem(int clientId, const void* pData, int dataLen)
{
    g_Logger.Log(MR_LOG_DEBUG, "Client %d: MRNM_CREATE_MAIN_ELEM", clientId);
    // TODO: Add player to race
}

void MessageDispatcher::Handle_SetMainElemState(int clientId, const void* pData, int dataLen)
{
    g_Logger.Log(MR_LOG_DEBUG, "Client %d: MRNM_SET_MAIN_ELEM_STATE", clientId);
    // TODO: Update player position/state
}

void MessageDispatcher::Handle_ChatMessage(int clientId, const void* pData, int dataLen)
{
    g_Logger.Log(MR_LOG_DEBUG, "Client %d: MRNM_CHAT_MESSAGE", clientId);
    // TODO: Broadcast chat to race
}

void MessageDispatcher::Handle_Ready(int clientId, const void* pData, int dataLen)
{
    g_Logger.Log(MR_LOG_DEBUG, "Client %d: MRNM_READY", clientId);
    // TODO: Mark player as ready
}
