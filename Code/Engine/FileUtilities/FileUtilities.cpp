#include <io.h>

#include "Engine/FileUtilities/FileUtilities.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"



FileUtilities* g_FileUtilities = nullptr;



FileUtilities::FileUtilities()
{

}



bool FileUtilities::LoadBinaryFileToBuffer(const char* filePath, std::vector<unsigned char>& outBuffer)
{
	FILE* file = nullptr;
	errno_t result = fopen_s(&file, filePath, "rb");

	if (result != 0)
	{
		return false;
	}
	else
	{
		fseek(file, 0, SEEK_END);
		int fileByteSize = ftell(file);
		rewind(file);
		outBuffer.resize(fileByteSize);
		fread(&outBuffer[0], sizeof(unsigned char), fileByteSize, file);
		fclose(file);
	}

	return true;
}



bool FileUtilities::SaveBufferToBinaryFile(const char* filePath, const std::vector<unsigned char>& buffer)
{
	FILE* file = nullptr;
	errno_t result = fopen_s(&file, filePath, "wb");

	if (result != 0)
	{
		return false;
	}
	else
	{
		fwrite(&buffer[0], sizeof(unsigned char), buffer.size(), file);
		fclose(file);
	}

	return true;
}



bool FileUtilities::LoadTextFileToBuffer(const char* filePath, std::string& outBuffer)
{
	FILE* file = nullptr;
	errno_t result = fopen_s(&file, filePath, "r");

	if (result != 0)
	{
		return false;
	}
	else
	{
		fseek(file, 0, SEEK_END);
		int fileByteSize = ftell(file);
		rewind(file);
		outBuffer.resize(fileByteSize);
		fread(&outBuffer[0], sizeof(char), fileByteSize, file);
		fclose(file);
	}

	return true;
}



bool FileUtilities::SaveBufferToTextFile(const char* filePath, const std::string& buffer)
{
	FILE* file = nullptr;
	errno_t result = fopen_s(&file, filePath, "w");

	if (result != 0)
	{
		return false;
	}
	else
	{
		fwrite(&buffer[0], sizeof(char), buffer.size(), file);
		fclose(file);
	}

	return true;
}



void FileUtilities::EnumerateFilesInDirectory(const std::string& relativeDirectoryPath, const std::string& filePattern, std::vector<std::string>& foundFileList, bool recurseSubFolders /*= false*/)
{
	std::string searchPattern = relativeDirectoryPath + "/" + filePattern;

	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst(searchPattern.c_str(), &fileInfo);

	while (searchHandle != -1 && !error)
	{
		std::string relativeFilePath = Stringf("%s/%s", relativeDirectoryPath.c_str(), fileInfo.name);

		bool resultIsADirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
		bool resultIsAHiddenFile = fileInfo.attrib & _A_HIDDEN ? true : false;

		if (!resultIsADirectory && !resultIsAHiddenFile)
		{
			foundFileList.push_back(relativeFilePath);
		}

		error = _findnext(searchHandle, &fileInfo);
	}
	_findclose(searchHandle);

	if (recurseSubFolders)
	{
		std::vector<std::string> subDirectories;
		EnumerateSubDirectoriesInDirectory(relativeDirectoryPath, subDirectories);

		for (const std::string& currentSubDirectory : subDirectories)
		{
			EnumerateFilesInDirectory(currentSubDirectory, filePattern, foundFileList, recurseSubFolders);
		}
	}
}



void FileUtilities::EnumerateSubDirectoriesInDirectory(const std::string& relativeDirectoryPath, std::vector<std::string>& subDirectoriesList)
{
	std::string searchPattern = relativeDirectoryPath + "/*";

	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst(searchPattern.c_str(), &fileInfo);

	while (searchHandle != -1 && !error)
	{
		std::string relativeFilePath = Stringf("%s/%s", relativeDirectoryPath.c_str(), fileInfo.name);

		bool resultIsADirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
		bool resultIsAHiddenFile = fileInfo.attrib & _A_HIDDEN ? true : false;

		if (resultIsADirectory && !resultIsAHiddenFile)
		{
			if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
			{
				subDirectoriesList.push_back(relativeFilePath);
			}
		}

		error = _findnext(searchHandle, &fileInfo);
	}
	_findclose(searchHandle);
}