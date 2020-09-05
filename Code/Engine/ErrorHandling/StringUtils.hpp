#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

void GetSubstringsWithDelimiter(const std::string& compositeString, const std::string& stringDelimiter, std::vector<std::string>& resultantSubStrings);
void CopyString(char* destinationString, const char* sourceString, size_t destinationLength);



struct CompareCStrings
{
	bool operator()(const char* firstString, const char* secondString) const;
};