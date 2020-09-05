#pragma once

#include <WinSock2.h>

#include "Engine/Networking/UDP/NetBytePacker.hpp"



const size_t MESSAGE_MTU = 1024;



enum NetworkMessageType : uint8_t
{
	NET_MESSAGE_PING,
	NET_MESSAGE_PONG,
	NET_MESSAGE_JOIN_REQUEST,
	NET_MESSAGE_JOIN_DENY,
	NET_MESSAGE_JOIN_ACCEPT,
	NET_MESSAGE_LEAVE,
	NET_MESSAGE_NEXT,
	INVALID_MESSAGE_TYPE = 255
};



enum NetworkMessageControl : uint8_t
{
	NMC_CONNECTED = 0,
	NMC_CONNECTIONLESS = (1 << 0),
	INVALID_CONTROL_TYPE = 255
};



enum NetworkMessageOption : uint8_t
{
	NMO_UNRELIABLE = 0,
	NMO_RELIABLE = (1 << 0),
	NMO_SEQUENCED = (1 << 1),
	INVALID_OPTION_TYPE = 255
};



struct NetSender
{
	class NetSession* m_Session;
	class NetConnection* m_Connection;
	sockaddr_in m_Address;
};



class NetMessage : public NetBytePacker
{
public:
	NetMessage();
	NetMessage(uint8_t messageID);

	size_t GetHeaderSize() const;
	size_t GetPayloadSize() const;
	size_t GetTotalMessageSize() const;

	bool RequiresConnection() const;
	bool IsReliable() const;
	bool IsInSequence() const;

	void ProcessMessage(const NetSender& fromSender) const;

public:
	uint8_t m_MessageID;
	unsigned char m_Buffer[MESSAGE_MTU];
	class NetMessageDefinition* m_MessageDefinition;

	uint16_t m_ReliableID;
	uint32_t m_LastSentElapsedTime;
	
	uint16_t m_SequenceID;
};



typedef void (NetMessageCallBack)(const NetSender&, const NetMessage&);



class NetMessageDefinition
{
public:
	NetMessageDefinition(uint8_t messageID = INVALID_MESSAGE_TYPE, uint8_t controlFlags = INVALID_CONTROL_TYPE, uint8_t optionFlags = INVALID_OPTION_TYPE, NetMessageCallBack* messageCallBack = nullptr);

public:
	uint8_t m_MessageID;
	uint8_t m_ControlFlags;
	uint8_t m_OptionFlags;
	NetMessageCallBack* m_MessageCallBack;
};