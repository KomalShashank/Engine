#pragma once

#include <WinSock2.h>



class UDPSocket
{
private:
	UDPSocket(const char* hostName, const char* servicePort);
	~UDPSocket();

public:
	static UDPSocket* CreateAndBindUDPSocket(const char* hostName, const char* servicePort);
	static void UnbindAndDestroyUDPSocket(UDPSocket*& currentUDPSocket);

	size_t SendToAddressOnSocket(const void* sendData, size_t dataSize, const sockaddr_in& toAddress);
	size_t ReceiveFromAddressOnSocket(void* dataBuffer, size_t bufferSize, sockaddr_in* fromAddress);

	bool IsBound() const;

public:
	SOCKET m_Socket;
	sockaddr_in m_Address;
};