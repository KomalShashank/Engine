#include "Engine/Networking/UDP/UDPSocket.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



UDPSocket::UDPSocket(const char* hostName, const char* servicePort)
{
	addrinfo* infoList = NetworkSystem::SingletonInstance()->AllocateAddressesForHost(hostName, servicePort, AF_INET, SOCK_DGRAM, AI_PASSIVE);

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



UDPSocket::~UDPSocket()
{
	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
}



UDPSocket* UDPSocket::CreateAndBindUDPSocket(const char* hostName, const char* servicePort)
{
	return new UDPSocket(hostName, servicePort);
}



void UDPSocket::UnbindAndDestroyUDPSocket(UDPSocket*& currentUDPSocket)
{
	if (currentUDPSocket != nullptr)
	{
		delete currentUDPSocket;
		currentUDPSocket = nullptr;
	}
}



size_t UDPSocket::SendToAddressOnSocket(const void* sendData, size_t dataSize, const sockaddr_in& toAddress)
{
	if (m_Socket != INVALID_SOCKET)
	{
		int sentSize = sendto(m_Socket, (const char*)sendData, (int)dataSize, 0, (sockaddr*)&toAddress, sizeof(sockaddr_in));
		if (sentSize > 0)
		{
			return (size_t)sentSize;
		}
	}

	return 0U;
}



size_t UDPSocket::ReceiveFromAddressOnSocket(void* dataBuffer, size_t bufferSize, sockaddr_in* fromAddress)
{
	if (m_Socket != INVALID_SOCKET)
	{
		sockaddr_storage theirAddress;
		size_t theirAddressSize = sizeof(theirAddress);

		int receivedSize = recvfrom(m_Socket, (char*)dataBuffer, (int)bufferSize, 0, (sockaddr*)&theirAddress, (int*)&theirAddressSize);
		if (receivedSize > 0)
		{
			ASSERT_OR_DIE(theirAddressSize == sizeof(sockaddr_in), "Size mismatch.");
			memcpy_s(fromAddress, sizeof(sockaddr_in), &theirAddress, theirAddressSize);

			return (size_t)receivedSize;
		}
	}

	return 0U;
}



bool UDPSocket::IsBound() const
{
	return (m_Socket != INVALID_SOCKET);
}