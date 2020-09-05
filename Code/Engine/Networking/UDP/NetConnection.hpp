#pragma once

#include <vector>
#include <queue>

#include "Engine/Networking/UDP/NetMessage.hpp"
#include "Engine/Networking/UDP/NetPacket.hpp"



#define INVALID_CONNNECTION_INDEX 0xFF



const size_t MAXIMUM_GUID_LENGTH = 32;
const size_t MAXIMUM_ACK_BUNDLES = 128;
const size_t MAXIMUM_RELIABLE_RANGE = 1024;
const size_t MAXIMUM_MESSAGE_AGE = 150;
class NetSession;



enum NetConnectionState : uint8_t
{
	UNCONFIRMED_CONNECTION = 0,
	CONFIRMED_CONNECTION = (1 << 0),
	LOCAL_CONNECTION = (1 << 1),
};



class AckBundle
{
public:
	AckBundle();

	void AddReliableID(uint16_t reliableID);
	
public:
	uint16_t m_AckID;
	std::vector<uint16_t> m_SentReliableIDs;
};



struct NetConnectionInfo
{
	uint8_t m_ConnectionIndex;
	sockaddr_in m_Address;
	char m_GlobalUniqueID[MAXIMUM_GUID_LENGTH];
};



class NetConnection
{
private:
	NetConnection(NetSession* parentSession, uint8_t connectionIndex, const sockaddr_in& connectionAddress, const char* globalUniqueID);
	~NetConnection();

public:
	static NetConnection* CreateNetConnection(NetSession* parentSession, uint8_t connectionIndex, const sockaddr_in& connectionAddress, const char* globalUniqueID);
	static void DestroyNetConnection(NetConnection*& currentNetConnection);

	void UpdateConnectionInfo(const NetConnectionInfo& currentConnectionInfo);
	NetConnectionInfo GetConnectionInfo() const;

	bool IsConnectionConnected() const;
	bool IsHostConnection() const;
	bool IsClientConnection() const;

	void AddMessageToSendQueue(NetMessage& currentMessage);
	void SendPacketToMyConnection();

	void MarkAsLocalConnection();
	void MarkConnectionConfirmed();
	bool IsConnectionConfirmed() const;

	bool IsOwnConnection() const;
	bool HasReceivedReliableID(uint16_t reliableID) const;

	void ProcessMessage(const NetSender& fromSender, const NetMessage& currentMessage);
	void MarkMessageReceived(const NetMessage& currentMessage);

	void MarkPacketReceived(const PacketHeader& packetHeader);

private:
	uint8_t ResendSentReliableMessages(NetPacket& currentPacket, AckBundle* ackBundle);
	uint8_t SendUnsentReliableMessages(NetPacket& currentPacket, AckBundle* ackBundle);
	uint8_t SendUnreliables(NetPacket& currentPacket);

	void MarkReliableIDReceived(uint16_t reliableID);

	bool IsReliableIDConfirmed(uint16_t reliableID) const;
	void RemoveConfirmedReliableID(uint16_t reliableID);
	bool CanSendNewReliableMessage() const;
	bool MessageIsOld(const NetMessage* currentMessage) const;

	void ConfirmAck(uint16_t ackID);
	void ConfirmReliableID(uint16_t reliableID);

	uint16_t GetNextReliableID();
	void RemoveAllReceivedReliableIDsLessThan(uint16_t reliableID);
	bool CyclicGreaterThanOrEqual(uint16_t firstValue, uint16_t secondValue);
	void UpdateHighestAckAndPreviousAckBitfield(uint16_t ackID);

	uint16_t GetNextSequenceID();
	void ProcessInSequence(const NetSender& fromSender, const NetMessage& currentMessage);
	
	void AddSequencedMessage(NetMessage* sequencedMessage);
	NetMessage* FindAndGetNextSequencedMessage();

	AckBundle* CreateAndGetAckBundle(uint16_t ackID);
	AckBundle* FindAckBundle(uint16_t ackID);
	uint16_t GetNextAck();

	void RemoveAllUnreliableMessages();

public:
	NetSession* m_ParentSession;
	uint8_t m_ConnectionIndex;
	uint8_t m_ConnectionState;
	sockaddr_in m_Address;
	char m_GlobalUniqueID[MAXIMUM_GUID_LENGTH];

	uint16_t m_NextSentAck;
	AckBundle m_AckBundles[MAXIMUM_ACK_BUNDLES];

	uint16_t m_HighestReceivedAck;
	uint16_t m_NextExpectedAck;
	uint16_t m_PreviousReceivedAcks;

	uint16_t m_NextSentReliableID;
	uint16_t m_OldestUnconfirmedReliableID;
	std::vector<uint16_t> m_ConfirmedReliableIDs;

	uint16_t m_NextExpectedReliableID;
	std::vector<uint16_t> m_ReceivedReliableIDs;

	uint16_t m_NextSentSequenceID;

	uint16_t m_NextExpectedSequenceID;
	std::vector<NetMessage*> m_OutOfOrderReceivedSequencedMessages;

	std::queue<NetMessage*> m_UnreliableMessages;
	std::queue<NetMessage*> m_UnsentReliableMessages;
	std::queue<NetMessage*> m_SentReliableMessages;
};