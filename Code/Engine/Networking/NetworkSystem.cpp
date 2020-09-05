#include "Engine/Networking/NetworkSystem.hpp"
#include "Engine/Networking/TCP-IP/RemoteCommandService.hpp"
#include "Engine/Networking/UDP/NetSession.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



const size_t BUFFER_SIZE = 1024;
NetworkSystem* g_NetworkSystem = nullptr;



NetworkSystem::NetworkSystem()
{
	
}



NetworkSystem* NetworkSystem::SingletonInstance()
{
	ASSERT_OR_DIE(g_NetworkSystem != nullptr, "A valid NetworkSystem instance does not exist.");

	return g_NetworkSystem;
}



bool NetworkSystem::InitializeNetworkSystem()
{
	if (g_NetworkSystem == nullptr)
	{
		g_NetworkSystem = new NetworkSystem();
		RemoteCommandService::CreateRemoteCommandService();
		NetSession::CreateNetSession();
	}

	WSADATA wsaData;
	int errorMessage = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (errorMessage == 0)
	{
		return true;
	}

	return false;
}



void NetworkSystem::UninitializeNetworkSystem()
{
	WSACleanup();
	
	if (g_NetworkSystem != nullptr)
	{
		NetSession::DestroyNetSession();
		delete g_NetworkSystem;
		g_NetworkSystem = nullptr;
	}
}



void NetworkSystem::Update()
{
	m_OnUpdate.TriggerEvent();
}



NetSession* NetworkSystem::GetLocalNetSession()
{
	return NetSession::LocalNetSession();
}



const char* NetworkSystem::GetLocalHostName() const
{
	static char localHostName[BUFFER_SIZE];
	if (gethostname(localHostName, BUFFER_SIZE) == 0)
	{
		return localHostName;
	}

	return "localhost";
}



const char* NetworkSystem::GetSocketAddressAsString(const sockaddr* socketAddress) const
{
	static char addressString[BUFFER_SIZE];
	sockaddr_in* ipv4Address = (sockaddr_in*)socketAddress;
	char hostName[BUFFER_SIZE];
	inet_ntop(ipv4Address->sin_family, GetAddressPartOfSocket(socketAddress), hostName, BUFFER_SIZE);
	sprintf_s(addressString, BUFFER_SIZE, "%s:%u", hostName, ntohs(ipv4Address->sin_port));

	return addressString;
}



const char* NetworkSystem::GetHostNameForAddress(const sockaddr* socketAddress) const
{
	static char hostName[BUFFER_SIZE];
	char servicePort[BUFFER_SIZE];

	getnameinfo(socketAddress, sizeof(sockaddr), hostName, BUFFER_SIZE, servicePort, BUFFER_SIZE, NI_NAMEREQD);

	return hostName;
}



const char* NetworkSystem::GetServicePortForAddress(const sockaddr* socketAddress) const
{
	char hostName[BUFFER_SIZE];
	static char servicePort[BUFFER_SIZE];

	getnameinfo(socketAddress, sizeof(sockaddr), hostName, BUFFER_SIZE, servicePort, BUFFER_SIZE, NI_NAMEREQD);

	return servicePort;
}



bool NetworkSystem::SocketShouldDisconnect(int32_t disconnectError) const
{
	switch (disconnectError)
	{
	case WSAEWOULDBLOCK:
	case WSAEMSGSIZE:
	case WSAECONNRESET:
		return false;

	default:
		return true;
	}
}



void* NetworkSystem::GetAddressPartOfSocket(const sockaddr* socketAddress) const
{
	if (socketAddress->sa_family == AF_INET)
	{
		return &(((sockaddr_in*)socketAddress)->sin_addr);
	}
	else
	{
		return &(((sockaddr_in6*)socketAddress)->sin6_addr);
	}
}



sockaddr_in NetworkSystem::GetSocketAddressFromString(const char* ipAddress, uint16_t servicePort) const
{
	sockaddr_in resultantAddress;
	memset(&resultantAddress, 0, sizeof(sockaddr_in));
	inet_pton(AF_INET, ipAddress, &(resultantAddress.sin_addr));
	resultantAddress.sin_port = htons(servicePort);
	resultantAddress.sin_family = AF_INET;

	return resultantAddress;
}



bool NetworkSystem::AreAddressesSame(const sockaddr_in& firstAddress, const sockaddr_in& secondAddress) const
{
	return ((firstAddress.sin_addr.S_un.S_addr == secondAddress.sin_addr.S_un.S_addr) && (firstAddress.sin_port == secondAddress.sin_port));
}



addrinfo* NetworkSystem::AllocateAddressesForHost(const char* hostName, const char* servicePort, int connectionFamily, int socketType, int hintFlags /*= 0*/)
{
	addrinfo addressHints;
	memset(&addressHints, 0, sizeof(addressHints));

	addressHints.ai_family = connectionFamily;
	addressHints.ai_socktype = socketType;
	addressHints.ai_flags = hintFlags;

	addrinfo* addressList = nullptr;
	int status = getaddrinfo(hostName, servicePort, &addressHints, &addressList);
	ASSERT_OR_DIE((status == 0 && addressList != nullptr), "No addresses were allocated for host.");

	return addressList;
}



void NetworkSystem::FreeAddresses(addrinfo* addressList)
{
	if (addressList != nullptr)
	{
		freeaddrinfo(addressList);
	}
}



void NetworkSystem::GetAllAddressesForPort(const char* servicePort, std::vector<std::string>& addressList)
{
	addrinfo* infoList = NetworkSystem::SingletonInstance()->AllocateAddressesForHost(GetLocalHostName(), servicePort, AF_INET, SOCK_STREAM, AI_PASSIVE);

	while (infoList != nullptr)
	{
		sockaddr* currentAddress = infoList->ai_addr;
		const char* currentAddressString = GetSocketAddressAsString(currentAddress);
		addressList.push_back(std::string(currentAddressString));

		infoList = infoList->ai_next;
	}
}