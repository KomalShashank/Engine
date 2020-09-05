#include "Engine/Networking/UDP/NetPacket.hpp"
#include "Engine/Networking/UDP/NetMessage.hpp"
#include "Engine/Networking/UDP/NetSession.hpp"



NetPacket::NetPacket() :
NetBytePacker(m_Buffer, 0, PACKET_MTU, BIG_ENDIAN)
{

}



void NetPacket::WritePacketHeader(const PacketHeader& packetHeader)
{
	Write<uint8_t>(packetHeader.m_SenderConnectionIndex);
	Write<uint16_t>(packetHeader.m_PacketAck);
	Write<uint16_t>(packetHeader.m_MostRecentAck);
	Write<uint16_t>(packetHeader.m_PreviouslyReceivedAcksBitfield);
}



void NetPacket::ReadPacketHeader(PacketHeader& packetHeader) const
{
	Read<uint8_t>(&packetHeader.m_SenderConnectionIndex);
	Read<uint16_t>(&packetHeader.m_PacketAck);
	Read<uint16_t>(&packetHeader.m_MostRecentAck);
	Read<uint16_t>(&packetHeader.m_PreviouslyReceivedAcksBitfield);
}



void NetPacket::WriteMessage(const NetMessage* currentMessage)
{
	Write<uint16_t>(static_cast<uint16_t>(currentMessage->GetTotalMessageSize()));
	Write<uint8_t>(currentMessage->m_MessageID);
	if (currentMessage->IsReliable())
	{
		Write<uint16_t>(currentMessage->m_ReliableID);
		if (currentMessage->IsInSequence())
		{
			Write<uint16_t>(currentMessage->m_SequenceID);
		}
	}
	WriteForward(currentMessage->m_Buffer, currentMessage->GetPayloadSize());
}



void NetPacket::ReadMessage(NetMessage* currentMessage)
{
	uint16_t messageSize;
	Read<uint16_t>(&messageSize);
	Read<uint8_t>(&currentMessage->m_MessageID);
	currentMessage->m_MessageDefinition = NetSession::LocalNetSession()->FindMessageDefinition(currentMessage->m_MessageID);
	if (currentMessage->IsReliable())
	{
		Read<uint16_t>(&currentMessage->m_ReliableID);
		if (currentMessage->IsInSequence())
		{
			Read<uint16_t>(&currentMessage->m_SequenceID);
		}
	}

	size_t payloadSize = static_cast<size_t>(messageSize) - currentMessage->GetHeaderSize() - sizeof(uint16_t);
	ReadForward(currentMessage->m_Buffer, payloadSize);
}



bool NetPacket::CanWriteMessage(const NetMessage* currentMessage) const
{
	return (GetWritableSize() >= currentMessage->GetTotalMessageSize());
}