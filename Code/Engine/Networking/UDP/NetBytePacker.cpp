#include <string>

#include "Engine/Networking/UDP/NetBytePacker.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



NetBytePacker::NetBytePacker(void* buffer, size_t maximumReadSize, size_t maximumWriteSize, const EndianMode& endianMode) :
m_Buffer(buffer),
m_MaximumReadSize(maximumReadSize),
m_MaximumWriteSize(maximumWriteSize),
m_EndianMode(endianMode),
m_Offset(0U)
{

}



void NetBytePacker::AdvanceWriteOffset(size_t offsetAmount)
{
	if (m_Offset + offsetAmount <= m_MaximumWriteSize)
	{
		m_Offset += offsetAmount;
	}
}



void NetBytePacker::AdvanceReadOffset(size_t offsetAmount) const
{
	if (m_Offset + offsetAmount <= m_MaximumReadSize)
	{
		m_Offset += offsetAmount;
	}
}



size_t NetBytePacker::GetReadableSize() const
{
	return GetTotalReadableSize() - m_Offset;
}



size_t NetBytePacker::GetTotalReadableSize() const
{
	return GetMaximumOfTwoSize_T(m_Offset, m_MaximumReadSize);
}



void NetBytePacker::SetReadableSize(size_t readableSize)
{
	m_MaximumReadSize = readableSize;
}



size_t NetBytePacker::GetWritableSize() const
{
	return GetTotalWritableSize() - m_Offset;
}



size_t NetBytePacker::GetTotalWritableSize() const
{
	return GetMaximumOfTwoSize_T(m_Offset, m_MaximumWriteSize);
}



void NetBytePacker::SetWritableSize(size_t writeableSize)
{
	m_MaximumWriteSize = writeableSize;
}



void* NetBytePacker::GetBufferHead() const
{
	return (unsigned char*)m_Buffer + m_Offset;
}



size_t NetBytePacker::GetOffset() const
{
	return m_Offset;
}



void NetBytePacker::ResetOffset() const
{
	m_Offset = 0U;
}



bool NetBytePacker::WriteForward(const void* dataBuffer, size_t dataSize)
{
	if (GetWritableSize() >= dataSize)
	{
		memcpy_s(GetBufferHead(), GetWritableSize(), dataBuffer, dataSize);
		AdvanceWriteOffset(dataSize);

		return true;
	}

	return false;
}



bool NetBytePacker::WriteBackward(const void* dataBuffer, size_t dataSize)
{
	void* bufferCopy = new void*;
	memcpy_s(bufferCopy, dataSize, dataBuffer, dataSize);
	ConvertByteDataToOppositeEndianMode(bufferCopy, dataSize);
	bool writeSuccess = WriteForward(bufferCopy, dataSize);
	delete bufferCopy;
	
	return writeSuccess;
}



void NetBytePacker::WriteAtLocation(const void* dataBuffer, size_t dataSize, void* bufferLocation)
{
	memcpy_s(bufferLocation, dataSize, dataBuffer, dataSize);
}



bool NetBytePacker::ReadForward(void* dataBuffer, size_t dataSize) const
{
	if (GetReadableSize() >= dataSize)
	{
		memcpy_s(dataBuffer, GetReadableSize(), GetBufferHead(), dataSize);
		AdvanceReadOffset(dataSize);

		return true;
	}
	
	return false;
}



bool NetBytePacker::ReadBackward(void* dataBuffer, size_t dataSize) const
{
	bool readSuccessful = ReadForward(dataBuffer, dataSize);
	if (readSuccessful)
	{
		ConvertByteDataToOppositeEndianMode(dataBuffer, dataSize);

		return true;
	}

	return false;
}



bool NetBytePacker::WriteString(const char* dataString)
{
	if (dataString == nullptr)
	{
		return Write<unsigned char>(INVALID_STRING);
	}
	else
	{
		size_t dataStringLength = strlen(dataString) + 1;
		return WriteForward(dataString, dataStringLength);
	}
}



const char* NetBytePacker::ReadString() const
{
	if (GetReadableSize() < 1)
	{
		return nullptr;
	}

	unsigned char nullCheck;
	if (!Read<unsigned char>(&nullCheck))
	{
		return nullptr;
	}

	if (nullCheck == INVALID_STRING)
	{
		return nullptr;
	}
	else
	{
		char* stringBuffer = (char*)GetBufferHead() - 1;
		size_t maximumSize = GetReadableSize() + 1;
		size_t stringLength = 0;

		char* currentCharacter = stringBuffer;
		while (stringLength < maximumSize && *currentCharacter != NULL)
		{
			++stringLength;
			++currentCharacter;
		}

		if (stringLength < maximumSize)
		{
			AdvanceReadOffset(stringLength);
			return stringBuffer;
		}
		else
		{
			return nullptr;
		}
	}
}



bool NetBytePacker::WriteVector2(const Vector2& writableVector2)
{
	return (WriteForward(&(writableVector2.X), sizeof(float)) && WriteForward(&(writableVector2.Y), sizeof(float)));
}



bool NetBytePacker::ReadVector2(Vector2& readableVector2) const
{
	return (ReadForward(&(readableVector2.X), sizeof(float)) && ReadForward(&(readableVector2.Y), sizeof(float)));
}



bool NetBytePacker::WriteRGBA(const RGBA& writableRGBA)
{
	return (Write<uint8_t>(writableRGBA.m_Red) && Write<uint8_t>(writableRGBA.m_Green) && Write<uint8_t>(writableRGBA.m_Blue) && Write<uint8_t>(writableRGBA.m_Alpha));
}



bool NetBytePacker::ReadRGBA(RGBA& readableRGBA) const
{
	return (Read<uint8_t>(&readableRGBA.m_Red) && Read<uint8_t>(&readableRGBA.m_Green) && Read<uint8_t>(&readableRGBA.m_Blue) && Read<uint8_t>(&readableRGBA.m_Alpha));
}