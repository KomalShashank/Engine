#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdint.h>

#include "Engine/EventSystem/EventSystem.hpp"
#include "Engine/Networking/UDP/NetSession.hpp"

#pragma comment(lib, "ws2_32.lib")



class NetworkSystem
{
private:
	NetworkSystem();

public:
	static NetworkSystem* SingletonInstance();

	static bool InitializeNetworkSystem();
	static void UninitializeNetworkSystem();

	void Update();

	NetSession* GetLocalNetSession();

	const char* GetLocalHostName() const;
	const char* GetSocketAddressAsString(const sockaddr* socketAddress) const;

	const char* GetHostNameForAddress(const sockaddr* socketAddress) const;
	const char* GetServicePortForAddress(const sockaddr* socketAddress) const;

	bool SocketShouldDisconnect(int32_t disconnectError) const;
	void* GetAddressPartOfSocket(const sockaddr* socketAddress) const;

	sockaddr_in GetSocketAddressFromString(const char* ipAddress, uint16_t servicePort) const;
	bool AreAddressesSame(const sockaddr_in& firstAddress, const sockaddr_in& secondAddress) const;

	addrinfo* AllocateAddressesForHost(const char* hostName, const char* servicePort, int connectionFamily, int socketType, int hintFlags = 0);
	void FreeAddresses(addrinfo* addressList);

	void GetAllAddressesForPort(const char* servicePort, std::vector<std::string>& addressList);

public:
	EventSystem<> m_OnUpdate;
};