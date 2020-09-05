#include "Engine/Tools/XMLUtilities.hpp"
#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/VectorMath/4D/Vector4.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/Math/VectorMath/3D/IntVector3.hpp"
#include "Engine/Math/VectorMath/4D/IntVector4.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"



uint8_t ParseUInt8FromXML(const char* XMLValue)
{
	return (uint8_t)atoi(XMLValue);
}



uint16_t ParseUInt16FromXML(const char* XMLValue)
{
	return (uint16_t)atoi(XMLValue);
}



uint32_t ParseUInt32FromXML(const char* XMLValue)
{
	return (uint32_t)atoi(XMLValue);
}



uint64_t ParseUInt64FromXML(const char* XMLValue)
{
	return (uint64_t)atoi(XMLValue);
}



float ParseFloatFromXML(const char* XMLValue)
{
	return (float)atof(XMLValue);
}



double ParseDoubleFromXML(const char* XMLValue)
{
	return (double)atof(XMLValue);
}



bool ParseBooleanFromXML(const char* XMLValue)
{
	if (strcmp(XMLValue, "True") == 0 || strcmp(XMLValue, "Yes") == 0)
	{
		return true;
	}
	else if (strcmp(XMLValue, "False") == 0 || strcmp(XMLValue, "No") == 0)
	{
		return false;
	}

	return false;
}



IntVector2 ParseIntVector2FromXML(const char* XMLValue)
{
	std::vector<std::string> IntVector2Components;
	GetSubstringsWithDelimiter(XMLValue, ",", IntVector2Components);

	IntVector2 resultantValue;
	resultantValue.X = std::stoi(IntVector2Components[0]);
	resultantValue.Y = std::stoi(IntVector2Components[1]);

	return resultantValue;
}



IntVector3 ParseIntVector3FromXML(const char* XMLValue)
{
	std::vector<std::string> IntVector3Components;
	GetSubstringsWithDelimiter(XMLValue, ",", IntVector3Components);

	IntVector3 resultantValue;
	resultantValue.X = std::stoi(IntVector3Components[0]);
	resultantValue.Y = std::stoi(IntVector3Components[1]);
	resultantValue.Z = std::stoi(IntVector3Components[2]);

	return resultantValue;
}



IntVector4 ParseIntVector4FromXML(const char* XMLValue)
{
	std::vector<std::string> IntVector4Components;
	GetSubstringsWithDelimiter(XMLValue, ",", IntVector4Components);

	IntVector4 resultantValue;
	resultantValue.X = std::stoi(IntVector4Components[0]);
	resultantValue.Y = std::stoi(IntVector4Components[1]);
	resultantValue.Z = std::stoi(IntVector4Components[2]);
	resultantValue.T = std::stoi(IntVector4Components[3]);

	return resultantValue;
}



Vector2 ParseVector2FromXML(const char* XMLValue)
{
	std::vector<std::string> Vector2Components;
	GetSubstringsWithDelimiter(XMLValue, ",", Vector2Components);

	Vector2 resultantValue;
	resultantValue.X = std::stof(Vector2Components[0]);
	resultantValue.Y = std::stof(Vector2Components[1]);

	return resultantValue;
}



Vector3 ParseVector3FromXML(const char* XMLValue)
{
	std::vector<std::string> Vector3Components;
	GetSubstringsWithDelimiter(XMLValue, ",", Vector3Components);

	Vector3 resultantValue;
	resultantValue.X = std::stof(Vector3Components[0]);
	resultantValue.Y = std::stof(Vector3Components[1]);
	resultantValue.Z = std::stof(Vector3Components[2]);

	return resultantValue;
}



Vector4 ParseVector4FromXML(const char* XMLValue)
{
	std::vector<std::string> Vector4Components;
	GetSubstringsWithDelimiter(XMLValue, ",", Vector4Components);

	Vector4 resultantValue;
	resultantValue.X = std::stof(Vector4Components[0]);
	resultantValue.Y = std::stof(Vector4Components[1]);
	resultantValue.Z = std::stof(Vector4Components[2]);
	resultantValue.T = std::stof(Vector4Components[3]);

	return resultantValue;
}



RGBA ParseRGBAFromXML(const char* XMLValue)
{
	std::vector<std::string> RGBAComponents;
	GetSubstringsWithDelimiter(XMLValue, ",", RGBAComponents);

	RGBA resultantValue;
	resultantValue.m_Red = (uint8_t)std::stoi(RGBAComponents[0]);
	resultantValue.m_Green = (uint8_t)std::stoi(RGBAComponents[1]);
	resultantValue.m_Blue = (uint8_t)std::stoi(RGBAComponents[2]);
	resultantValue.m_Alpha = (uint8_t)std::stoi(RGBAComponents[3]);

	return resultantValue;
}