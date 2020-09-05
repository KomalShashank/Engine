#pragma once

#include "Engine/Networking/UDP/NetBytePacker.hpp"



#define INVALID_PACKET_ACK 0xFFFF



const size_t PACKET_MTU = 1232;



struct PacketHeader
{
	uint8_t m_SenderConnectionIndex;
	uint16_t m_PacketAck;
	uint16_t m_MostRecentAck;
	uint16_t m_PreviouslyReceivedAcksBitfield;
};



class NetMessage;



class NetPacket : public NetBytePacker
{
public:
	NetPacket();

	void WritePacketHeader(const PacketHeader& packetHeader);
	void ReadPacketHeader(PacketHeader& packetHeader) const;

	void WriteMessage(const NetMessage* currentMessage);
	void ReadMessage(NetMessage* currentMessage);
	bool CanWriteMessage(const NetMessage* currentMessage) const;

public:
	unsigned char m_Buffer[PACKET_MTU];
};