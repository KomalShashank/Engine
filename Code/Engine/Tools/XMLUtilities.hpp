#pragma once

#include <stdint.h>

#include "ThirdParty/XMLParser/XMLParser.hpp"



class IntVector2;
class IntVector3;
class IntVector4;

class Vector2;
class Vector3;
class Vector4;

class RGBA;

uint8_t ParseUInt8FromXML(const char* XMLValue);
uint16_t ParseUInt16FromXML(const char* XMLValue);
uint32_t ParseUInt32FromXML(const char* XMLValue);
uint64_t ParseUInt64FromXML(const char* XMLValue);

float ParseFloatFromXML(const char* XMLValue);
double ParseDoubleFromXML(const char* XMLValue);

bool ParseBooleanFromXML(const char* XMLValue);

IntVector2 ParseIntVector2FromXML(const char* XMLValue);
IntVector3 ParseIntVector3FromXML(const char* XMLValue);
IntVector4 ParseIntVector4FromXML(const char* XMLValue);

Vector2 ParseVector2FromXML(const char* XMLValue);
Vector3 ParseVector3FromXML(const char* XMLValue);
Vector4 ParseVector4FromXML(const char* XMLValue);

RGBA ParseRGBAFromXML(const char* XMLValue);