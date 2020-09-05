#include "Engine/Networking/UDP/NetMessage.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



NetMessage::NetMessage() :
NetBytePacker(m_Buffer, MESSAGE_MTU, 0, BIG_ENDIAN),
m_MessageID(INVALID_MESSAGE_TYPE),
m_MessageDefinition(nullptr),
m_ReliableID(0),
m_LastSentElapsedTime(0),
m_SequenceID(0)
{

}



NetMessage::NetMessage(uint8_t messageID) :
NetBytePacker(m_Buffer, 0, MESSAGE_MTU, BIG_ENDIAN),
m_MessageID(messageID),
m_MessageDefinition(nullptr),
m_ReliableID(0),
m_LastSentElapsedTime(0),
m_SequenceID(0)
{

}



size_t NetMessage::GetHeaderSize() const
{
	size_t headerSize = sizeof(m_MessageID);
	if (IsReliable())
	{
		headerSize += sizeof(m_ReliableID);
		if (IsInSequence())
		{
			headerSize += sizeof(m_SequenceID);
		}
	}

	return headerSize;
}



size_t NetMessage::GetPayloadSize() const
{
	return GetTotalReadableSize();
}



size_t NetMessage::GetTotalMessageSize() const
{
	return (GetHeaderSize() + GetPayloadSize() + sizeof(uint16_t));
}



bool NetMessage::RequiresConnection() const
{
	ASSERT_OR_DIE(m_MessageDefinition != nullptr, "Message has no valid definition.");
	ASSERT_OR_DIE(m_MessageDefinition->m_ControlFlags != INVALID_CONTROL_TYPE, "Control type is undefined.");

	if (m_MessageDefinition->m_ControlFlags == NMC_CONNECTED)
	{
		return true;
	}
	else if (m_MessageDefinition->m_ControlFlags == NMC_CONNECTIONLESS)
	{
		return false;
	}

	return false;
}



bool NetMessage::IsReliable() const
{
	ASSERT_OR_DIE(m_MessageDefinition != nullptr, "Message has no valid definition.");
	ASSERT_OR_DIE(m_MessageDefinition->m_OptionFlags != INVALID_OPTION_TYPE, "Option type is undefined.");

	return IsBitSet(m_MessageDefinition->m_OptionFlags, NMO_RELIABLE);
}



bool NetMessage::IsInSequence() const
{
	ASSERT_OR_DIE(m_MessageDefinition != nullptr, "Message has no valid definition.");
	ASSERT_OR_DIE(m_MessageDefinition->m_OptionFlags != INVALID_OPTION_TYPE, "Option type is undefined.");

	return IsBitSet(m_MessageDefinition->m_OptionFlags, NMO_SEQUENCED);
}



void NetMessage::ProcessMessage(const NetSender& fromSender) const
{
	m_MessageDefinition->m_MessageCallBack(fromSender, *this);
}



NetMessageDefinition::NetMessageDefinition(uint8_t messageID /*= INVALID_MESSAGE_TYPE*/, uint8_t controlFlags /*= INVALID_CONTROL_TYPE*/, uint8_t optionFlags /*= INVALID_OPTION_TYPE*/, NetMessageCallBack* messageCallBack /*= nullptr*/) :
m_MessageID(messageID),
m_ControlFlags(controlFlags),
m_OptionFlags(optionFlags),
m_MessageCallBack(messageCallBack)
{

}