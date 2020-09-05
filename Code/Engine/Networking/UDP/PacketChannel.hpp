#pragma once

#include <vector>

#include "Engine/Networking/UDP/NetPacket.hpp"
#include "Engine/Networking/UDP/UDPSocket.hpp"
#include "Engine/DataStructures/ObjectPool.hpp"



struct TimeStampedPacket
{
	NetPacket m_NetPacket;
	double m_TimeToProcess;
	sockaddr_in m_FromAddress;
};



class PacketChannel
{
private:
	PacketChannel(const char* hostName, const char* servicePort);
	~PacketChannel();

public:
	static PacketChannel* CreatePacketChannel(const char* hostName, const char* servicePort);
	static void DestroyPacketChannel(PacketChannel*& currentPacketChannel);

	size_t SendToAddressOnPacketChannel(const void* sendData, size_t dataSize, const sockaddr_in& toAddress);
	size_t ReceiveFromAddressOnPacketChannel(void* dataBuffer, size_t bufferSize, sockaddr_in* fromAddress); // TODO: Complete and verify this function.

	bool IsChannelActive() const;

public:
	UDPSocket* m_UDPSocket;

	std::vector<TimeStampedPacket*> m_InboundPackets;
	ObjectPool<TimeStampedPacket> m_PacketPool;

	float m_DropPercentage;
	double m_MinimumLag;
	double m_MaximumLag;
};