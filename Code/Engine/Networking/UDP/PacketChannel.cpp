#include "Engine/Networking/UDP/PacketChannel.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"
#include "Engine/Time/Time.hpp"



const size_t MAXIMUM_NUMBER_OF_PACKETS = 1024;



PacketChannel::PacketChannel(const char* hostName, const char* servicePort) :
m_DropPercentage(0.0f),
m_MinimumLag(0.0),
m_MaximumLag(0.0)
{
	m_UDPSocket = UDPSocket::CreateAndBindUDPSocket(hostName, servicePort);
	m_PacketPool.InitializeObjectPool(MAXIMUM_NUMBER_OF_PACKETS);
}



PacketChannel::~PacketChannel()
{
	m_PacketPool.UninitializeObjectPool();
	
	if (IsChannelActive())
	{
		UDPSocket::UnbindAndDestroyUDPSocket(m_UDPSocket);
	}
}



PacketChannel* PacketChannel::CreatePacketChannel(const char* hostName, const char* servicePort)
{
	return new PacketChannel(hostName, servicePort);
}



void PacketChannel::DestroyPacketChannel(PacketChannel*& currentPacketChannel)
{
	if (currentPacketChannel != nullptr)
	{
		delete currentPacketChannel;
		currentPacketChannel = nullptr;
	}
}



size_t PacketChannel::SendToAddressOnPacketChannel(const void* sendData, size_t dataSize, const sockaddr_in& toAddress)
{
	return m_UDPSocket->SendToAddressOnSocket(sendData, dataSize, toAddress);
}



size_t PacketChannel::ReceiveFromAddressOnPacketChannel(void* dataBuffer, size_t bufferSize, sockaddr_in* fromAddress)
{
	TimeStampedPacket* currentPacket = m_PacketPool.AllocateObjectFromPool();
	sockaddr_in sendingAddress;
	size_t receivedSize = m_UDPSocket->ReceiveFromAddressOnSocket(currentPacket->m_NetPacket.m_Buffer, bufferSize, &sendingAddress);
	if (receivedSize > 0)
	{
		if (GetRandomFloatWithinRange(0.0f, 1.0f) < m_DropPercentage)
		{
			m_PacketPool.DeallocateObjectToPool(currentPacket);
		}
		else
		{
			double packetDelay = GetRandomDoubleWithinRange(m_MinimumLag, m_MaximumLag);
			currentPacket->m_NetPacket.SetReadableSize(receivedSize);
			currentPacket->m_TimeToProcess = GetCurrentTimeInMilliseconds() + packetDelay;
			currentPacket->m_FromAddress = sendingAddress;
			m_InboundPackets.push_back(currentPacket);
		}
	}
	else
	{
		m_PacketPool.DeallocateObjectToPool(currentPacket);
	}

	if (m_InboundPackets.size() > 0)
	{
		double currentTime = GetCurrentTimeInMilliseconds();
		currentPacket = m_InboundPackets[0];
		if (currentTime >= currentPacket->m_TimeToProcess)
		{
			*fromAddress = currentPacket->m_FromAddress;
			size_t dataSize = currentPacket->m_NetPacket.GetTotalReadableSize();
			memcpy(dataBuffer, currentPacket->m_NetPacket.m_Buffer, dataSize);
			m_PacketPool.DeallocateObjectToPool(currentPacket);
			m_InboundPackets.erase(m_InboundPackets.begin());

			return dataSize;
		}
	}

	return 0U;
}



bool PacketChannel::IsChannelActive() const
{
	if (m_UDPSocket != nullptr)
	{
		if (m_UDPSocket->IsBound())
		{
			return true;
		}
	}

	return false;
}