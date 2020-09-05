#include "Engine/ErrorHandling/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}



void GetSubstringsWithDelimiter(const std::string& compositeString, const std::string& stringDelimiter, std::vector<std::string>& resultantSubStrings)
{
	if (compositeString.empty())
	{
		return;
	}

	auto stringStartingPosition = 0U;
	auto stringEndingPosition = compositeString.find(stringDelimiter);

	std::string subString;
	while (stringEndingPosition != std::string::npos)
	{
		subString = compositeString.substr(stringStartingPosition, stringEndingPosition - stringStartingPosition);
		resultantSubStrings.push_back(subString);

		stringStartingPosition = stringEndingPosition + stringDelimiter.length();
		stringEndingPosition = compositeString.find(stringDelimiter, stringStartingPosition);
	}

	subString = compositeString.substr(stringStartingPosition, stringEndingPosition);
	resultantSubStrings.push_back(subString);
}



void CopyString(char* destinationString, const char* sourceString, size_t destinationLength)
{
	size_t sourceLength = strlen(sourceString);
	size_t copyLength = (sourceLength < destinationLength - 1) ? sourceLength : destinationLength - 1;

	strncpy_s(destinationString, destinationLength, sourceString, copyLength);
	destinationString[copyLength] = '\0';
}



bool CompareCStrings::operator()(const char* firstString, const char* secondString) const
{
	return (strcmp(firstString, secondString) < 0);
}