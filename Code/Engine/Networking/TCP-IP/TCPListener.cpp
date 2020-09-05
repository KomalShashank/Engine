#include "Engine/Networking/TCP-IP/TCPListener.hpp"
#include "Engine/Networking/TCP-IP/TCPConnection.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



TCPListener::TCPListener(const char* hostName, const char* servicePort, int queueCount /*= 1*/)
{
	addrinfo* infoList = NetworkSystem::SingletonInstance()->AllocateAddressesForHost(hostName, servicePort, AF_INET, SOCK_STREAM, AI_PASSIVE);

	m_Socket = INVALID_SOCKET;
	addrinfo* infoListIterator = infoList;
	while ((infoListIterator != nullptr) && (m_Socket == INVALID_SOCKET))
	{
		m_Socket = socket(infoListIterator->ai_family, infoListIterator->ai_socktype, infoListIterator->ai_protocol);
		if (m_Socket != INVALID_SOCKET)
		{
			int bindResult = bind(m_Socket, infoListIterator->ai_addr, infoListIterator->ai_addrlen);
			if (bindResult != SOCKET_ERROR)
			{
				u_long nonBlocking = 1;
				ioctlsocket(m_Socket, FIONBIO, &nonBlocking);
				bindResult = listen(m_Socket, queueCount);
				ASSERT_OR_DIE(bindResult != SOCKET_ERROR, "Socket Error.");

				memcpy_s(&m_Address, sizeof(sockaddr_in), infoListIterator->ai_addr, infoListIterator->ai_addrlen);
			}
			else
			{
				closesocket(m_Socket);
				m_Socket = INVALID_SOCKET;
			}
		}

		infoListIterator = infoListIterator->ai_next;
	}

	NetworkSystem::SingletonInstance()->FreeAddresses(infoList);
}



TCPListener::TCPListener(const char* servicePort, int queueCount /*= 1*/)
{
	TCPListener(NetworkSystem::SingletonInstance()->GetLocalHostName(), servicePort, queueCount);
}



void TCPListener::StopListener()
{
	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
}



bool TCPListener::IsListening()
{
	return (m_Socket != INVALID_SOCKET);
}



TCPConnection* TCPListener::AcceptConnection()
{
	sockaddr_storage theirAddress;
	size_t theirAddressSize = sizeof(theirAddress);

	SOCKET newSocket = accept(m_Socket, (sockaddr*)&theirAddress, (int*)&theirAddressSize);
	if (newSocket != INVALID_SOCKET)
	{
		sockaddr_in newAddress;
		ASSERT_OR_DIE(theirAddressSize == sizeof(sockaddr_in), "Size mismatch.");
		memcpy_s(&newAddress, sizeof(sockaddr_in), &theirAddress, theirAddressSize);

		return new TCPConnection(newSocket, newAddress);
	}

	return nullptr;
}