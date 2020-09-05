#pragma once

#include <stdint.h>
#include <string>

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/MatrixMath/Matrix4.hpp"



enum EndianMode
{
	LITTLE_ENDIAN,
	BIG_ENDIAN
};



inline EndianMode GetLocalEndianness()
{
	union
	{
		unsigned char byteData[4];
		uint32_t integerData;
	} data;

	data.integerData = 0x04030201;

	return (data.byteData[0] == 0x01) ? LITTLE_ENDIAN : BIG_ENDIAN;
}



inline void ConvertByteDataToOppositeEndianMode(void* byteData, const size_t numberOfBytes)
{
	unsigned char* byteArray = reinterpret_cast<unsigned char*>(byteData);
	unsigned char temporaryByte;

	for (size_t byteIndex = 0; byteIndex < numberOfBytes / 2; ++byteIndex)
	{
		size_t reverseIndex = numberOfBytes - (byteIndex + 1);

		temporaryByte = byteArray[byteIndex];
		byteArray[byteIndex] = byteArray[reverseIndex];
		byteArray[reverseIndex] = temporaryByte;
	}

	byteData = reinterpret_cast<void*>(byteArray);
}



class BinaryWriteInterface
{
public:
	BinaryWriteInterface();

	virtual size_t WriteBytes(const void* byteData, const size_t numberOfBytes) const = 0;

	void SetEndianness(const EndianMode& endianMode);

	bool WriteBinaryString(const std::string& stringData) const;
	bool WriteTextString(const std::string& stringData) const;

	bool WriteVector2(const Vector2& vector2Data) const;
	bool WriteVector3(const Vector3& vector3Data) const;
	bool WriteMatrix4(const Matrix4& matrix4Data) const;

	template <typename dataType>
	bool Write(const dataType& dataValue) const
	{
		dataType dataCopy = dataValue;
		if (GetLocalEndianness() != m_EndianMode)
		{
			ConvertByteDataToOppositeEndianMode(&dataCopy, sizeof(dataType));
		}

		return (WriteBytes(&dataCopy, sizeof(dataType)) == sizeof(dataType));
	}

private:
	EndianMode m_EndianMode;
};



class BinaryReadInterface
{
public:
	BinaryReadInterface();

	virtual size_t ReadBytes(void* byteData, const size_t numberOfBytes) const = 0;

	void SetEndianness(const EndianMode& endianMode);

	bool ReadBinaryString(std::string& stringData) const;

	bool ReadVector2(Vector2& vector2Data) const;
	bool ReadVector3(Vector3& vector3Data) const;
	bool ReadMatrix4(Matrix4& matrix4Data) const;

	template <typename dataType>
	bool Read(dataType& dataValue) const
	{
		if (ReadBytes(&dataValue, sizeof(dataType)) == sizeof(dataType))
		{
			if (GetLocalEndianness() != m_EndianMode)
			{
				ConvertByteDataToOppositeEndianMode(&dataValue, sizeof(dataType));
			}

			return true;
		}

		return false;
	}

private:
	EndianMode m_EndianMode;
};