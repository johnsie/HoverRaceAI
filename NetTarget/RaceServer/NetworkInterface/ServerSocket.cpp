// ServerSocket.cpp : Server socket implementation

#include "stdafx.h"
#include "ServerSocket.h"
#include "MessageDispatcher.h"
#include "RaceManager.h"
#include "ServerLogger.h"

extern MR_ServerLogger g_Logger;

MR_ServerSocket::MR_ServerSocket()
    : mListenSocket(INVALID_SOCKET),
      mDatagramSocket(INVALID_SOCKET),
      mNextClientId(1),
      mMaxConnections(40),
      mPort(9600)
{
}

MR_ServerSocket::~MR_ServerSocket()
{
    Shutdown();
}

BOOL MR_ServerSocket::Initialize(unsigned port, int maxConnections)
{
    mPort = port;
    mMaxConnections = maxConnections;

    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        g_Logger.Log(MR_LOG_ERROR, "WSAStartup failed: %d", iResult);
        return FALSE;
    }

    // Create TCP listening socket
    mListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mListenSocket == INVALID_SOCKET) {
        g_Logger.Log(MR_LOG_ERROR, "TCP socket creation failed: %ld", WSAGetLastError());
        WSACleanup();
        return FALSE;
    }

    // Create UDP datagram socket
    mDatagramSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (mDatagramSocket == INVALID_SOCKET) {
        g_Logger.Log(MR_LOG_ERROR, "UDP socket creation failed: %ld", WSAGetLastError());
        closesocket(mListenSocket);
        WSACleanup();
        return FALSE;
    }

    // Set socket options
    if (!SetSocketOptions(mListenSocket) || !SetSocketOptions(mDatagramSocket)) {
        g_Logger.Log(MR_LOG_ERROR, "Failed to set socket options");
        closesocket(mListenSocket);
        closesocket(mDatagramSocket);
        WSACleanup();
        return FALSE;
    }

    // Bind TCP socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if (bind(mListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        g_Logger.Log(MR_LOG_ERROR, "TCP bind failed: %ld", WSAGetLastError());
        closesocket(mListenSocket);
        closesocket(mDatagramSocket);
        WSACleanup();
        return FALSE;
    }

    // Bind UDP socket
    if (bind(mDatagramSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        g_Logger.Log(MR_LOG_ERROR, "UDP bind failed: %ld", WSAGetLastError());
        closesocket(mListenSocket);
        closesocket(mDatagramSocket);
        WSACleanup();
        return FALSE;
    }

    // Listen on TCP socket
    if (listen(mListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        g_Logger.Log(MR_LOG_ERROR, "listen() failed: %ld", WSAGetLastError());
        closesocket(mListenSocket);
        closesocket(mDatagramSocket);
        WSACleanup();
        return FALSE;
    }

    g_Logger.Log(MR_LOG_INFO, "Server socket initialized successfully on port %u", port);
    return TRUE;
}

BOOL MR_ServerSocket::SetSocketOptions(SOCKET sock)
{
    // Enable SO_REUSEADDR to allow quick rebind after crash
    int reuseAddr = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddr, sizeof(reuseAddr)) == SOCKET_ERROR) {
        g_Logger.Log(MR_LOG_WARN, "SO_REUSEADDR failed: %ld", WSAGetLastError());
    }

    // Disable Nagle's algorithm for TCP (low-latency requirement)
    if (sock != mDatagramSocket) {
        int tcpNoDelay = 1;
        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&tcpNoDelay, sizeof(tcpNoDelay)) == SOCKET_ERROR) {
            g_Logger.Log(MR_LOG_WARN, "TCP_NODELAY failed: %ld", WSAGetLastError());
        }
    }

    // Set send buffer size
    int sendBufSize = 8192;
    if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize)) == SOCKET_ERROR) {
        g_Logger.Log(MR_LOG_WARN, "SO_SNDBUF failed: %ld", WSAGetLastError());
    }

    // Set receive buffer size
    int recvBufSize = 8192;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(recvBufSize)) == SOCKET_ERROR) {
        g_Logger.Log(MR_LOG_WARN, "SO_RCVBUF failed: %ld", WSAGetLastError());
    }

    return TRUE;
}

void MR_ServerSocket::ProcessEvents(MR_RaceManager* pRaceManager)
{
    // Accept new incoming connections
    AcceptNewConnection();

    // Process messages from existing connections
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(mDatagramSocket, &readSet);

    // Build fd_set for all client connections
    for (auto& pair : mConnections) {
        ClientConnection* pConn = pair.second;
        if (pConn && pConn->mConnected) {
            FD_SET(pConn->mTcpSocket, &readSet);
        }
    }

    // Non-blocking select to check for ready sockets
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;  // 1ms timeout

    int selectResult = select(0, &readSet, NULL, NULL, &timeout);
    if (selectResult > 0) {
        // Check UDP datagram socket
        if (FD_ISSET(mDatagramSocket, &readSet)) {
            ReceiveDatagram();
        }

        // Check each TCP client connection
        std::vector<int> clientsToRemove;
        for (auto& pair : mConnections) {
            int clientId = pair.first;
            ClientConnection* pConn = pair.second;
            if (pConn && FD_ISSET(pConn->mTcpSocket, &readSet)) {
                ReceiveFromClient(pConn);
                if (!pConn->IsAlive()) {
                    clientsToRemove.push_back(clientId);
                }
            }
        }

        // Remove dead connections
        for (int clientId : clientsToRemove) {
            CloseConnection(clientId);
        }
    }
}

void MR_ServerSocket::AcceptNewConnection()
{
    fd_set listenSet;
    FD_ZERO(&listenSet);
    FD_SET(mListenSocket, &listenSet);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;  // Non-blocking

    int selectResult = select(0, &listenSet, NULL, NULL, &timeout);
    if (selectResult <= 0) {
        return;  // No pending connections
    }

    if (!FD_ISSET(mListenSocket, &listenSet)) {
        return;
    }

    // Check if we have room for more connections
    if ((int)mConnections.size() >= mMaxConnections) {
        // Too many connections, accept and immediately close
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(mListenSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
        }
        return;
    }

    // Accept new connection
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(mListenSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (clientSocket == INVALID_SOCKET) {
        g_Logger.Log(MR_LOG_WARN, "accept() failed: %ld", WSAGetLastError());
        return;
    }

    // Create new client connection
    ClientConnection* pNewConn = new ClientConnection();
    pNewConn->mClientId = mNextClientId++;
    pNewConn->mTcpSocket = clientSocket;
    pNewConn->mConnected = TRUE;
    pNewConn->mConnectTime = time(NULL);
    pNewConn->mLastMessageTime = pNewConn->mConnectTime;
    pNewConn->mUdpAddr = clientAddr;

    mConnections[pNewConn->mClientId] = pNewConn;

    g_Logger.Log(MR_LOG_INFO, "New client connection: ID=%d from %s:%u",
                pNewConn->mClientId,
                inet_ntoa(clientAddr.sin_addr),
                ntohs(clientAddr.sin_port));
}

void MR_ServerSocket::ReceiveFromClient(ClientConnection* pConn)
{
    // TODO: Implement TCP message receive
    // Read MR_NetMessageBuffer from pConn->mTcpSocket
    // Update pConn->mLastMessageTime
}

void MR_ServerSocket::ReceiveDatagram()
{
    // TODO: Implement UDP datagram receive
    // Read datagram from mDatagramSocket
}

void MR_ServerSocket::BroadcastToRace(
    int raceId,
    const void* pMessageData,
    int messageLength,
    int excludeClientId)
{
    // TODO: Send message to all clients in specified race
}

void MR_ServerSocket::SendToPlayer(
    int clientId,
    const void* pMessageData,
    int messageLength)
{
    // TODO: Send message to specific client
}

void MR_ServerSocket::CloseConnection(int clientId)
{
    auto it = mConnections.find(clientId);
    if (it != mConnections.end()) {
        ClientConnection* pConn = it->second;
        g_Logger.Log(MR_LOG_INFO, "Closing connection: ID=%d", clientId);
        if (pConn->mTcpSocket != INVALID_SOCKET) {
            closesocket(pConn->mTcpSocket);
        }
        delete pConn;
        mConnections.erase(it);
    }
}

void MR_ServerSocket::Shutdown()
{
    // Close all client connections
    for (auto& pair : mConnections) {
        ClientConnection* pConn = pair.second;
        if (pConn) {
            if (pConn->mTcpSocket != INVALID_SOCKET) {
                closesocket(pConn->mTcpSocket);
            }
            delete pConn;
        }
    }
    mConnections.clear();

    // Close server sockets
    if (mListenSocket != INVALID_SOCKET) {
        closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
    }
    if (mDatagramSocket != INVALID_SOCKET) {
        closesocket(mDatagramSocket);
        mDatagramSocket = INVALID_SOCKET;
    }

    WSACleanup();
    g_Logger.Log(MR_LOG_INFO, "Server socket shutdown complete");
}
