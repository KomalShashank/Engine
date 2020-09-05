#pragma once

#include "Engine/IO Utilities/BinaryIO.hpp"



class BinaryFileWriter : public BinaryWriteInterface
{
public:
	BinaryFileWriter();

	bool OpenBinaryFile(const char* fileName, bool appendToExistingFile = false);
	bool FlushBinaryFile();
	void CloseBinaryFile();

	virtual size_t WriteBytes(const void* byteData, const size_t numberOfBytes) const override;

private:
	FILE* m_File;
};



class BinaryFileReader : public BinaryReadInterface
{
public:
	BinaryFileReader();

	bool OpenBinaryFile(const char* fileName);
	size_t GetBinaryFileSize();
	void CloseBinaryFile();

	virtual size_t ReadBytes(void* byteData, const size_t numberOfBytes) const override;

private:
	FILE* m_File;
};