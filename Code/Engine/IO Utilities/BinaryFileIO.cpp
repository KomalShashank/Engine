#include "Engine/IO Utilities/BinaryFileIO.hpp"



BinaryFileWriter::BinaryFileWriter() :
BinaryWriteInterface(),
m_File(nullptr)
{

}



bool BinaryFileWriter::OpenBinaryFile(const char* fileName, bool appendToExistingFile /*= false*/)
{
	const char* fileMode = (appendToExistingFile) ? "ab" : "wb";
	errno_t result = fopen_s(&m_File, fileName, fileMode);

	return (result == 0);
}



bool BinaryFileWriter::FlushBinaryFile()
{
	errno_t result;
	if (m_File != nullptr)
	{
		result = fflush(m_File);

		return (result == 0);
	}

	return false;
}



void BinaryFileWriter::CloseBinaryFile()
{
	if (m_File != nullptr)
	{
		fclose(m_File);
		m_File = nullptr;
	}
}



size_t BinaryFileWriter::WriteBytes(const void* byteData, const size_t numberOfBytes) const
{
	return fwrite(byteData, sizeof(unsigned char), numberOfBytes, m_File);
}



BinaryFileReader::BinaryFileReader() :
BinaryReadInterface(),
m_File(nullptr)
{

}



bool BinaryFileReader::OpenBinaryFile(const char* fileName)
{
	errno_t result = fopen_s(&m_File, fileName, "rb");

	return (result == 0);
}



size_t BinaryFileReader::GetBinaryFileSize()
{
	fseek(m_File, 0, SEEK_END);
	size_t binaryFileSize = ftell(m_File);
	rewind(m_File);

	return binaryFileSize;
}



void BinaryFileReader::CloseBinaryFile()
{
	if (m_File != nullptr)
	{
		fclose(m_File);
		m_File = nullptr;
	}
}



size_t BinaryFileReader::ReadBytes(void* byteData, const size_t numberOfBytes) const
{
	return fread(byteData, sizeof(unsigned char), numberOfBytes, m_File);
}