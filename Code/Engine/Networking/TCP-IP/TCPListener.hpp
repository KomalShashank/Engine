#pragma once

#include "Engine/Networking/NetworkSystem.hpp"



class TCPConnection;



class TCPListener
{
public:
	TCPListener(const char* hostName, const char* servicePort, int queueCount = 1);
	TCPListener(const char* servicePort, int queueCount = 1);

	void StopListener();
	bool IsListening();

	TCPConnection* AcceptConnection();

public:
	SOCKET m_Socket;
	sockaddr_in m_Address;
};