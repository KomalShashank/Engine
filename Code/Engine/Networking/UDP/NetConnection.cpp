#include "Engine/Networking/UDP/NetConnection.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"
#include "Engine/Time/Time.hpp"



AckBundle::AckBundle() :
m_AckID(INVALID_PACKET_ACK)
{

}



void AckBundle::AddReliableID(uint16_t reliableID)
{
	for (const uint16_t& currentID : m_SentReliableIDs)
	{
		if (currentID == reliableID)
		{
			return;
		}
	}

	m_SentReliableIDs.push_back(reliableID);
}



NetConnection::NetConnection(NetSession* parentSession, uint8_t connectionIndex, const sockaddr_in& connectionAddress, const char* globalUniqueID) :
m_ParentSession(parentSession),
m_ConnectionIndex(connectionIndex),
m_ConnectionState(UNCONFIRMED_CONNECTION),
m_Address(connectionAddress),
m_NextSentAck(0U),
m_HighestReceivedAck(INVALID_PACKET_ACK),
m_NextExpectedAck(m_HighestReceivedAck + 1),
m_PreviousReceivedAcks(0U),
m_NextSentReliableID(0U),
m_OldestUnconfirmedReliableID(0U),
m_NextExpectedReliableID(0U),
m_NextSentSequenceID(0U),
m_NextExpectedSequenceID(0U)
{
	CopyString(m_GlobalUniqueID, globalUniqueID, MAXIMUM_GUID_LENGTH);
}



NetConnection::~NetConnection()
{

}



NetConnection* NetConnection::CreateNetConnection(NetSession* parentSession, uint8_t connectionIndex, const sockaddr_in& connectionAddress, const char* globalUniqueID)
{
	return new NetConnection(parentSession, connectionIndex, connectionAddress, globalUniqueID);
}



void NetConnection::DestroyNetConnection(NetConnection*& currentNetConnection)
{
	if (currentNetConnection != nullptr)
	{
		delete currentNetConnection;
		currentNetConnection = nullptr;
	}
}



void NetConnection::UpdateConnectionInfo(const NetConnectionInfo& currentConnectionInfo)
{
	m_ConnectionIndex = currentConnectionInfo.m_ConnectionIndex;
	m_Address.sin_port = currentConnectionInfo.m_Address.sin_port;
	m_Address.sin_addr.S_un.S_addr = currentConnectionInfo.m_Address.sin_addr.S_un.S_addr;
	CopyString(m_GlobalUniqueID, currentConnectionInfo.m_GlobalUniqueID, MAXIMUM_GUID_LENGTH);
}



NetConnectionInfo NetConnection::GetConnectionInfo() const
{
	NetConnectionInfo connectionInfo;
	connectionInfo.m_ConnectionIndex = m_ConnectionIndex;
	connectionInfo.m_Address = m_Address;
	CopyString(connectionInfo.m_GlobalUniqueID, m_GlobalUniqueID, MAXIMUM_GUID_LENGTH);

	return connectionInfo;
}



bool NetConnection::IsConnectionConnected() const
{
	return (m_ConnectionIndex != INVALID_CONNNECTION_INDEX);
}



bool NetConnection::IsHostConnection() const // TODO: Verify with Forseth.
{
	return (m_ParentSession->GetHostConnection() == this);
}



bool NetConnection::IsClientConnection() const // TODO: Verify with Forseth.
{
	return !IsHostConnection();
}



void NetConnection::AddMessageToSendQueue(NetMessage& currentMessage)
{
	currentMessage.m_MessageDefinition = m_ParentSession->FindMessageDefinition(currentMessage.m_MessageID);

	NetMessage* messageCopy = new NetMessage(currentMessage);
	if (messageCopy->IsReliable())
	{
		if (messageCopy->IsInSequence())
		{
			messageCopy->m_SequenceID = GetNextSequenceID();
		}
		
		m_UnsentReliableMessages.push(messageCopy);
	}
	else
	{
		m_UnreliableMessages.push(messageCopy);
	}
}



void NetConnection::SendPacketToMyConnection()
{
	NetPacket packetToSend;

	PacketHeader packetHeader;
	packetHeader.m_SenderConnectionIndex = m_ParentSession->GetLocalConnectionIndex();
	packetHeader.m_PacketAck = GetNextAck();
	packetHeader.m_MostRecentAck = m_HighestReceivedAck;
	packetHeader.m_PreviouslyReceivedAcksBitfield = m_PreviousReceivedAcks;

	packetToSend.WritePacketHeader(packetHeader);

	uint8_t* messageCount = packetToSend.ReserveLocation<uint8_t>(0U);
	AckBundle* ackBundle = CreateAndGetAckBundle(packetHeader.m_PacketAck);

	uint8_t numberOfMessages = 0;
	numberOfMessages += ResendSentReliableMessages(packetToSend, ackBundle);
	numberOfMessages += SendUnsentReliableMessages(packetToSend, ackBundle);
	numberOfMessages += SendUnreliables(packetToSend);
	
	RemoveAllUnreliableMessages();
	*messageCount = numberOfMessages;

	if (numberOfMessages > 0)
	{
		size_t sentSize = m_ParentSession->m_PacketChannel->SendToAddressOnPacketChannel(packetToSend.m_Buffer, packetToSend.GetTotalReadableSize(), m_Address);
		if (sentSize > 0)
		{
			m_ParentSession->m_ElapsedTimeSinceLastSent = 0.0f;
		}
	}
}



void NetConnection::MarkAsLocalConnection()
{
	SetBits(m_ConnectionState, LOCAL_CONNECTION);
}



void NetConnection::MarkConnectionConfirmed()
{
	SetBits(m_ConnectionState, CONFIRMED_CONNECTION);
}



bool NetConnection::IsConnectionConfirmed() const
{
	return IsBitSet(m_ConnectionState, CONFIRMED_CONNECTION);
}



bool NetConnection::IsOwnConnection() const
{
	if (m_ParentSession->GetLocalConnection() != nullptr)
	{
		return (m_ParentSession->GetLocalConnection() == this);
	}
	
	return NetworkSystem::SingletonInstance()->AreAddressesSame(m_Address, m_ParentSession->GetSessionAddress());
}



bool NetConnection::HasReceivedReliableID(uint16_t reliableID) const
{
	if (reliableID > (m_NextExpectedReliableID - static_cast<uint16_t>(MAXIMUM_RELIABLE_RANGE)) && reliableID < m_NextExpectedReliableID)
	{
		for (const uint16_t& currentID : m_ReceivedReliableIDs)
		{
			if (currentID == reliableID)
			{
				return true;
			}
		}
	}

	return false;
}



void NetConnection::ProcessMessage(const NetSender& fromSender, const NetMessage& currentMessage)
{
	if (currentMessage.IsInSequence())
	{
		ProcessInSequence(fromSender, currentMessage);
	}
	else
	{
		currentMessage.ProcessMessage(fromSender);
	}
}



void NetConnection::MarkMessageReceived(const NetMessage& currentMessage)
{
	if (currentMessage.IsReliable())
	{
		MarkReliableIDReceived(currentMessage.m_ReliableID);
	}
}



void NetConnection::MarkPacketReceived(const PacketHeader& packetHeader)
{
	UpdateHighestAckAndPreviousAckBitfield(packetHeader.m_PacketAck);
	ConfirmAck(packetHeader.m_MostRecentAck);
	for (size_t bitIndex = 0; bitIndex < sizeof(packetHeader.m_PreviouslyReceivedAcksBitfield); ++bitIndex)
	{
		if (IsBitSetAtIndex(packetHeader.m_PreviouslyReceivedAcksBitfield, bitIndex))
		{
			ConfirmAck(packetHeader.m_MostRecentAck - (static_cast<uint16_t>(bitIndex) + 1));
		}
	}
}



uint8_t NetConnection::ResendSentReliableMessages(NetPacket& currentPacket, AckBundle* ackBundle)
{
	uint8_t numberOfMessages = 0;

	while (!m_SentReliableMessages.empty())
	{
		NetMessage* currentMessage = m_SentReliableMessages.front();
		if (IsReliableIDConfirmed(currentMessage->m_ReliableID))
		{
			m_SentReliableMessages.pop();
			delete currentMessage;
			continue;
		}

		if (MessageIsOld(currentMessage) && currentPacket.CanWriteMessage(currentMessage))
		{
			m_SentReliableMessages.pop();
			currentMessage->m_LastSentElapsedTime = static_cast<uint32_t>(GetCurrentTimeInMilliseconds());
			currentPacket.WriteMessage(currentMessage);
			++numberOfMessages;
			ackBundle->AddReliableID(currentMessage->m_ReliableID);
			m_SentReliableMessages.push(currentMessage);
		}
		else
		{
			break;
		}
	}

	return numberOfMessages;
}



uint8_t NetConnection::SendUnsentReliableMessages(NetPacket& currentPacket, AckBundle* ackBundle)
{
	uint8_t numberOfMessages = 0;

	while (!m_UnsentReliableMessages.empty() && CanSendNewReliableMessage())
	{
		NetMessage* currentMessage = m_UnsentReliableMessages.front();
		if (currentPacket.CanWriteMessage(currentMessage))
		{
			currentMessage->m_ReliableID = GetNextReliableID();
			currentMessage->m_LastSentElapsedTime = static_cast<uint32_t>(GetCurrentTimeInMilliseconds());
			currentPacket.WriteMessage(currentMessage);
			++numberOfMessages;
			ackBundle->AddReliableID(currentMessage->m_ReliableID);
			m_UnsentReliableMessages.pop();
			m_SentReliableMessages.push(currentMessage);
		}
		else
		{
			break;
		}
	}

	return numberOfMessages;
}



uint8_t NetConnection::SendUnreliables(NetPacket& currentPacket)
{
	uint8_t numberOfMessages = 0;

	while (!m_UnreliableMessages.empty())
	{
		NetMessage* currentMessage = m_UnreliableMessages.front();
		if (currentPacket.CanWriteMessage(currentMessage))
		{
			currentPacket.WriteMessage(currentMessage);
			++numberOfMessages;
			m_UnreliableMessages.pop();
		}
		else
		{
			break;
		}
	}

	return numberOfMessages;
}



void NetConnection::MarkReliableIDReceived(uint16_t reliableID)
{
	if (CyclicGreaterThanOrEqual(reliableID, m_NextExpectedReliableID))
	{
		uint16_t difference = reliableID - m_NextExpectedReliableID;
		ASSERT_OR_DIE(difference <= MAXIMUM_RELIABLE_RANGE, "Reliable ID is out of range.");

		m_NextExpectedReliableID = reliableID + 1;
		RemoveAllReceivedReliableIDsLessThan(reliableID - MAXIMUM_RELIABLE_RANGE);
	}
	else
	{
		uint16_t difference = m_NextExpectedReliableID - reliableID;
		if (difference < MAXIMUM_RELIABLE_RANGE)
		{
			m_ReceivedReliableIDs.push_back(reliableID);
		}
	}
}



bool NetConnection::IsReliableIDConfirmed(uint16_t reliableID) const
{
	for (const uint16_t& currentID : m_ConfirmedReliableIDs)
	{
		if (currentID == reliableID)
		{
			return true;
		}
	}

	if (reliableID < m_OldestUnconfirmedReliableID)
	{
		return true;
	}

	return false;
}



void NetConnection::RemoveConfirmedReliableID(uint16_t reliableID)
{
	for (auto reliableIDIterator = m_ConfirmedReliableIDs.begin(); reliableIDIterator != m_ConfirmedReliableIDs.end(); ++reliableIDIterator)
	{
		uint16_t currentID = *reliableIDIterator;
		if (currentID == reliableID)
		{
			m_ConfirmedReliableIDs.erase(reliableIDIterator);
			break;
		}
	}
}



bool NetConnection::CanSendNewReliableMessage() const
{
	return (m_NextSentReliableID - m_OldestUnconfirmedReliableID < MAXIMUM_RELIABLE_RANGE);
}



bool NetConnection::MessageIsOld(const NetMessage* currentMessage) const
{
	uint32_t currentTime = static_cast<uint32_t>(GetCurrentTimeInMilliseconds());
	
	return ((currentTime - currentMessage->m_LastSentElapsedTime) > MAXIMUM_MESSAGE_AGE);
}



void NetConnection::ConfirmAck(uint16_t ackID)
{
	if (ackID == INVALID_PACKET_ACK)
	{
		return;
	}
	
	AckBundle* ackBundle = FindAckBundle(ackID);
	if (ackBundle != nullptr)
	{
		for (const uint16_t& currentID : ackBundle->m_SentReliableIDs)
		{
			ConfirmReliableID(currentID);
		}
	}
}



void NetConnection::ConfirmReliableID(uint16_t reliableID)
{
	if (reliableID < m_OldestUnconfirmedReliableID)
	{
		return;
	}
	else if (reliableID == m_OldestUnconfirmedReliableID)
	{
		m_ConfirmedReliableIDs.push_back(reliableID);
		while (IsReliableIDConfirmed(m_OldestUnconfirmedReliableID))
		{
			RemoveConfirmedReliableID(m_OldestUnconfirmedReliableID);
			++m_OldestUnconfirmedReliableID;
		}
	}
	else
	{
		m_ConfirmedReliableIDs.push_back(reliableID);
	}
}



uint16_t NetConnection::GetNextReliableID()
{
	uint16_t currentID = m_NextSentReliableID;
	++m_NextSentReliableID;

	return currentID;
}



void NetConnection::RemoveAllReceivedReliableIDsLessThan(uint16_t reliableID)
{
	for (auto reliableIDIterator = m_ReceivedReliableIDs.begin(); reliableIDIterator != m_ReceivedReliableIDs.end();)
	{
		uint16_t currentID = *reliableIDIterator;
		if (currentID < reliableID)
		{
			reliableIDIterator = m_ReceivedReliableIDs.erase(reliableIDIterator);
			continue;
		}

		++reliableIDIterator;
	}
}



bool NetConnection::CyclicGreaterThanOrEqual(uint16_t firstValue, uint16_t secondValue)
{
	const uint16_t HALF_RANGE = 0x7FFF;
	uint16_t difference = firstValue - secondValue;

	return ((difference > 0) && (difference <= HALF_RANGE));
}



void NetConnection::UpdateHighestAckAndPreviousAckBitfield(uint16_t ackID)
{
	if (CyclicGreaterThanOrEqual(ackID, m_HighestReceivedAck))
	{
		uint16_t shiftOffset = ackID - m_HighestReceivedAck;
		m_PreviousReceivedAcks = m_PreviousReceivedAcks << shiftOffset;
		m_HighestReceivedAck = ackID;
		m_NextExpectedAck = m_HighestReceivedAck + 1;
		SetBitAtIndex(m_PreviousReceivedAcks, shiftOffset - 1);
	}
	else
	{
		uint16_t shiftOffset = m_HighestReceivedAck - ackID;
		SetBitAtIndex(m_PreviousReceivedAcks, shiftOffset - 1);
	}
}



uint16_t NetConnection::GetNextSequenceID()
{
	uint16_t currentID = m_NextSentSequenceID;
	++m_NextSentSequenceID;

	return currentID;
}



void NetConnection::ProcessInSequence(const NetSender& fromSender, const NetMessage& currentMessage)
{
	if (m_NextExpectedSequenceID == currentMessage.m_SequenceID)
	{
		currentMessage.ProcessMessage(fromSender);
		++m_NextExpectedSequenceID;

		NetMessage* sequencedMessage = FindAndGetNextSequencedMessage();
		while (sequencedMessage != nullptr)
		{
			sequencedMessage->ProcessMessage(fromSender);
			delete sequencedMessage;
			++m_NextExpectedSequenceID;

			sequencedMessage = FindAndGetNextSequencedMessage();
		}
	}
	else
	{
		NetMessage* messageCopy = new NetMessage(currentMessage);
		AddSequencedMessage(messageCopy);
	}
}



void NetConnection::AddSequencedMessage(NetMessage* sequencedMessage)
{
	for (const NetMessage* currentMessage : m_OutOfOrderReceivedSequencedMessages)
	{
		if (currentMessage->m_SequenceID == sequencedMessage->m_SequenceID)
		{
			delete sequencedMessage;
			return;
		}
	}

	m_OutOfOrderReceivedSequencedMessages.push_back(sequencedMessage);
}



NetMessage* NetConnection::FindAndGetNextSequencedMessage()
{
	for (auto sequencedMessageIterator = m_OutOfOrderReceivedSequencedMessages.begin(); sequencedMessageIterator != m_OutOfOrderReceivedSequencedMessages.end(); ++sequencedMessageIterator)
	{
		NetMessage* currentMessage = *sequencedMessageIterator;
		if (currentMessage->m_SequenceID == m_NextExpectedSequenceID)
		{
			m_OutOfOrderReceivedSequencedMessages.erase(sequencedMessageIterator);
			return currentMessage;
		}
	}

	return nullptr;
}



AckBundle* NetConnection::CreateAndGetAckBundle(uint16_t ackID)
{
	uint16_t bundleIndex = ackID % MAXIMUM_ACK_BUNDLES;
	AckBundle* ackBundle = &(m_AckBundles[bundleIndex]);
	ackBundle->m_AckID = ackID;
	ackBundle->m_SentReliableIDs.clear();

	return ackBundle;
}



AckBundle* NetConnection::FindAckBundle(uint16_t ackID)
{
	for (size_t bundleIndex = 0; bundleIndex < MAXIMUM_ACK_BUNDLES; ++bundleIndex)
	{
		if (m_AckBundles[bundleIndex].m_AckID == ackID)
		{
			return &m_AckBundles[bundleIndex];
		}
	}

	return nullptr;
}



uint16_t NetConnection::GetNextAck()
{
	uint16_t currentAck = m_NextSentAck;
	++m_NextSentAck;
	if (m_NextSentAck == INVALID_PACKET_ACK)
	{
		++m_NextSentAck;
	}

	return currentAck;
}



void NetConnection::RemoveAllUnreliableMessages()
{
	while (!m_UnreliableMessages.empty())
	{
		NetMessage* currentMessage = m_UnreliableMessages.front();
		delete currentMessage;
		m_UnreliableMessages.pop();
	}
}