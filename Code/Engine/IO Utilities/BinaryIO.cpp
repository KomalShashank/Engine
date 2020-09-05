#include <string.h>

#include "Engine/IO Utilities/BinaryIO.hpp"



BinaryWriteInterface::BinaryWriteInterface()
{
	SetEndianness(GetLocalEndianness());
}



void BinaryWriteInterface::SetEndianness(const EndianMode& endianMode)
{
	m_EndianMode = endianMode;
}



bool BinaryWriteInterface::WriteBinaryString(const std::string& stringData) const
{
	const char* stringArray = stringData.c_str();

	if (stringArray == nullptr)
	{
		Write<uint32_t>(0U);

		return true;
	}

	size_t stringArraySize = strlen(stringArray) + 1;

	return Write<uint32_t>(stringArraySize) && (WriteBytes(stringArray, stringArraySize) == stringArraySize);
}



bool BinaryWriteInterface::WriteTextString(const std::string& stringData) const
{
	const char* stringArray = stringData.c_str();

	if (stringArray != nullptr)
	{
		size_t stringArraySize = strlen(stringArray);
		WriteBytes(stringArray, stringArraySize);

		return true;
	}

	return false;
}



bool BinaryWriteInterface::WriteVector2(const Vector2& vector2Data) const
{
	bool success = true;

	success = success && Write<float>(vector2Data.X);
	success = success && Write<float>(vector2Data.Y);

	return success;
}



bool BinaryWriteInterface::WriteVector3(const Vector3& vector3Data) const
{
	bool success = true;

	success = success && Write<float>(vector3Data.X);
	success = success && Write<float>(vector3Data.Y);
	success = success && Write<float>(vector3Data.Z);

	return success;
}



bool BinaryWriteInterface::WriteMatrix4(const Matrix4& matrix4Data) const
{
	bool success = true;

	success = success && Write<float>(matrix4Data.m_Matrix4[0]);
	success = success && Write<float>(matrix4Data.m_Matrix4[1]);
	success = success && Write<float>(matrix4Data.m_Matrix4[2]);
	success = success && Write<float>(matrix4Data.m_Matrix4[3]);

	success = success && Write<float>(matrix4Data.m_Matrix4[4]);
	success = success && Write<float>(matrix4Data.m_Matrix4[5]);
	success = success && Write<float>(matrix4Data.m_Matrix4[6]);
	success = success && Write<float>(matrix4Data.m_Matrix4[7]);

	success = success && Write<float>(matrix4Data.m_Matrix4[8]);
	success = success && Write<float>(matrix4Data.m_Matrix4[9]);
	success = success && Write<float>(matrix4Data.m_Matrix4[10]);
	success = success && Write<float>(matrix4Data.m_Matrix4[11]);

	success = success && Write<float>(matrix4Data.m_Matrix4[12]);
	success = success && Write<float>(matrix4Data.m_Matrix4[13]);
	success = success && Write<float>(matrix4Data.m_Matrix4[14]);
	success = success && Write<float>(matrix4Data.m_Matrix4[15]);

	return success;
}



BinaryReadInterface::BinaryReadInterface()
{
	SetEndianness(GetLocalEndianness());
}



void BinaryReadInterface::SetEndianness(const EndianMode& endianMode)
{
	m_EndianMode = endianMode;
}



bool BinaryReadInterface::ReadBinaryString(std::string& stringData) const
{
	char* stringArray = nullptr;

	uint32_t stringArraySize;
	if (Read<uint32_t>(stringArraySize))
	{
		if (stringArraySize != 0U)
		{
			stringArray = new char[stringArraySize];
			if (ReadBytes(stringArray, stringArraySize) == stringArraySize)
			{
				stringData = std::string(stringArray);
				delete[] stringArray;

				return true;
			}

			delete[] stringArray;
		}
	}

	return false;
}



bool BinaryReadInterface::ReadVector2(Vector2& vector2Data) const
{
	bool success = true;

	success = success && Read<float>(vector2Data.X);
	success = success && Read<float>(vector2Data.Y);

	return success;
}



bool BinaryReadInterface::ReadVector3(Vector3& vector3Data) const
{
	bool success = true;

	success = success && Read<float>(vector3Data.X);
	success = success && Read<float>(vector3Data.Y);
	success = success && Read<float>(vector3Data.Z);

	return success;
}



bool BinaryReadInterface::ReadMatrix4(Matrix4& matrix4Data) const
{
	bool success = true;

	success = success && Read<float>(matrix4Data.m_Matrix4[0]);
	success = success && Read<float>(matrix4Data.m_Matrix4[1]);
	success = success && Read<float>(matrix4Data.m_Matrix4[2]);
	success = success && Read<float>(matrix4Data.m_Matrix4[3]);

	success = success && Read<float>(matrix4Data.m_Matrix4[4]);
	success = success && Read<float>(matrix4Data.m_Matrix4[5]);
	success = success && Read<float>(matrix4Data.m_Matrix4[6]);
	success = success && Read<float>(matrix4Data.m_Matrix4[7]);

	success = success && Read<float>(matrix4Data.m_Matrix4[8]);
	success = success && Read<float>(matrix4Data.m_Matrix4[9]);
	success = success && Read<float>(matrix4Data.m_Matrix4[10]);
	success = success && Read<float>(matrix4Data.m_Matrix4[11]);

	success = success && Read<float>(matrix4Data.m_Matrix4[12]);
	success = success && Read<float>(matrix4Data.m_Matrix4[13]);
	success = success && Read<float>(matrix4Data.m_Matrix4[14]);
	success = success && Read<float>(matrix4Data.m_Matrix4[15]);

	return success;
}