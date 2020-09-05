#pragma once

#include "Engine/Networking/NetworkSystem.hpp"



class TCPConnection
{
public:
	TCPConnection(const SOCKET& newSocket, const sockaddr_in& newAddress);
	TCPConnection(const char* hostName, const char* servicePort);

	void DisconnectTCP();

	size_t SendOnSocket(const void* sendData, size_t dataSize, bool& shouldDisconnect);
	size_t ReceiveOnSocket(void* dataBuffer, size_t bufferSize, bool& shouldDisconnect);

	bool IsTCPConnected();

public:
	SOCKET m_Socket;
	sockaddr_in m_Address;
};