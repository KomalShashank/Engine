#pragma once

#include <string>
#include <vector>



class FileUtilities
{
public:
	FileUtilities();

	bool LoadBinaryFileToBuffer(const char* filePath, std::vector<unsigned char>& outBuffer);
	bool SaveBufferToBinaryFile(const char* filePath, const std::vector<unsigned char>& buffer);

	bool LoadTextFileToBuffer(const char* filePath, std::string& outBuffer);
	bool SaveBufferToTextFile(const char* filePath, const std::string& buffer);

	void EnumerateFilesInDirectory(const std::string& relativeDirectoryPath, const std::string& filePattern, std::vector<std::string>& foundFileList, bool recurseSubFolders = false);
	void EnumerateSubDirectoriesInDirectory(const std::string& relativeDirectoryPath, std::vector<std::string>& subDirectoriesList);
};

extern FileUtilities* g_FileUtilities;