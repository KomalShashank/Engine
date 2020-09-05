#pragma once

#include "Engine/IO Utilities/BinaryIO.hpp"
#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"



const unsigned char INVALID_STRING = 0xff;



class NetBytePacker
{
public:
	NetBytePacker(void* buffer, size_t maximumReadSize, size_t maximumWriteSize, const EndianMode& endianMode);

	void AdvanceWriteOffset(size_t offsetAmount);
	void AdvanceReadOffset(size_t offsetAmount) const;

	size_t GetReadableSize() const;
	size_t GetTotalReadableSize() const;
	void SetReadableSize(size_t readableSize);

	size_t GetWritableSize() const;
	size_t GetTotalWritableSize() const;
	void SetWritableSize(size_t writeableSize);

	void* GetBufferHead() const;
	size_t GetOffset() const;
	void ResetOffset() const;

	template <typename data_type>
	bool Write(const data_type& dataBuffer)
	{
		size_t dataSize = sizeof(data_type);
		if (GetLocalEndianness() == m_EndianMode)
		{
			return WriteForward(&dataBuffer, dataSize);
		}
		else
		{
			return WriteBackward(&dataBuffer, dataSize);
		}
	}

	template <typename data_type>
	bool Read(data_type* dataBuffer) const
	{
		size_t dataSize = sizeof(data_type);
		if (GetLocalEndianness() == m_EndianMode)
		{
			return ReadForward(dataBuffer, dataSize);
		}
		else
		{
			return ReadBackward(dataBuffer, dataSize);
		}
	}

	bool WriteForward(const void* dataBuffer, size_t dataSize);
	bool WriteBackward(const void* dataBuffer, size_t dataSize);

	void WriteAtLocation(const void* dataBuffer, size_t dataSize, void* bufferLocation);
	
	template <typename data_type>
	data_type* ReserveLocation(const data_type& dataBuffer)
	{
		void* bufferHeadLocation = GetBufferHead();
		Write<data_type>(dataBuffer);

		return (data_type*)bufferHeadLocation;
	}

	bool ReadForward(void* dataBuffer, size_t dataSize) const;
	bool ReadBackward(void* dataBuffer, size_t dataSize) const;

	bool WriteString(const char* dataString);
	const char* ReadString() const;

	bool WriteVector2(const Vector2& writableVector2);
	bool ReadVector2(Vector2& readableVector2) const;

	bool WriteRGBA(const RGBA& writableRGBA);
	bool ReadRGBA(RGBA& readableRGBA) const;

private:
	void* m_Buffer;
	size_t m_MaximumReadSize;
	size_t m_MaximumWriteSize;
	EndianMode m_EndianMode;

private:
	mutable size_t m_Offset;
};