#include "Engine/Networking/TCP-IP/TCPConnection.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



TCPConnection::TCPConnection(const SOCKET& newSocket, const sockaddr_in& newAddress) :
m_Socket(newSocket),
m_Address(newAddress)
{
	
}



TCPConnection::TCPConnection(const char* hostName, const char* servicePort)
{
	addrinfo* infoList = NetworkSystem::SingletonInstance()->AllocateAddressesForHost(hostName, servicePort, AF_INET, SOCK_STREAM, 0);

	m_Socket = INVALID_SOCKET;
	addrinfo* infoListIterator = infoList;
	while ((infoListIterator != nullptr) && (m_Socket == INVALID_SOCKET))
	{
		m_Socket = socket(infoListIterator->ai_family, infoListIterator->ai_socktype, infoListIterator->ai_protocol);
		if (m_Socket != INVALID_SOCKET)
		{
			int connectResult = connect(m_Socket, infoListIterator->ai_addr, infoListIterator->ai_addrlen);
			if (connectResult != SOCKET_ERROR)
			{
				u_long nonBlocking = 1;
				ioctlsocket(m_Socket, FIONBIO, &nonBlocking);
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



void TCPConnection::DisconnectTCP()
{
	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
}



size_t TCPConnection::SendOnSocket(const void* sendData, size_t dataSize, bool& shouldDisconnect)
{
	shouldDisconnect = false;
	if (m_Socket != INVALID_SOCKET)
	{
		int sentSize = send(m_Socket, (const char*)sendData, (int)dataSize, 0);
		if (sentSize < 0)
		{
			int32_t socketError = WSAGetLastError();
			if (NetworkSystem::SingletonInstance()->SocketShouldDisconnect(socketError))
			{
				shouldDisconnect = true;
			}
		}
		else
		{
			ASSERT_OR_DIE((size_t)sentSize == dataSize, "Sent size is not equal to data size.");
		}

		return (size_t)sentSize;
	}

	return 0U;
}



size_t TCPConnection::ReceiveOnSocket(void* dataBuffer, size_t bufferSize, bool& shouldDisconnect)
{
	shouldDisconnect = false;
	if (m_Socket != INVALID_SOCKET)
	{
		int receivedSize = recv(m_Socket, (char*)dataBuffer, bufferSize, 0);
		if (receivedSize < 0)
		{
			int32_t socketError = WSAGetLastError();
			if (NetworkSystem::SingletonInstance()->SocketShouldDisconnect(socketError))
			{
				shouldDisconnect = true;
			}

			return 0U;
		}
		else
		{
			return (size_t)receivedSize;
		}
	}

	return 0U;
}



bool TCPConnection::IsTCPConnected()
{
	return (m_Socket != INVALID_SOCKET);
}