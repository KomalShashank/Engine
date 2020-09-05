#pragma once

#include <stdint.h>



enum WidgetPropertyType : uint8_t
{
	PROPERTY_TYPE_UINT8,
	PROPERTY_TYPE_UINT16,
	PROPERTY_TYPE_UINT32,
	PROPERTY_TYPE_UINT64,
	PROPERTY_TYPE_FLOAT,
	PROPERTY_TYPE_DOUBLE,
	PROPERTY_TYPE_BOOLEAN,
	PROPERTY_TYPE_STRING,
	PROPERTY_TYPE_INTVECTOR2,
	PROPERTY_TYPE_INTVECTOR3,
	PROPERTY_TYPE_INTVECTOR4,
	PROPERTY_TYPE_VECTOR2,
	PROPERTY_TYPE_VECTOR3,
	PROPERTY_TYPE_VECTOR4,
	PROPERTY_TYPE_COLOR,
	PROPERTY_TYPE_TEXTURE,
	PROPERTY_TYPE_FONT,
	NUMBER_OF_PROPERTY_TYPES,
	INVALID_PROPERTY_TYPE = 255U
};



class WidgetProperty
{
public:
	constexpr WidgetProperty(const char* widgetPropertyName, uint8_t widgetPropertyType) :
		m_WidgetPropertyName(widgetPropertyName),
		m_WidgetPropertyType(widgetPropertyType)
	{

	}

	const char* GetWidgetPropertyName() const;
	uint8_t GetWidgetPropertyType() const;

	static uint8_t GetWidgetPropertyTypeForPropertyName(const char* widgetPropertyName);

public:
	const char* m_WidgetPropertyName;
	uint8_t m_WidgetPropertyType;
};



constexpr WidgetProperty s_WidgetProperties[] =
{
	WidgetProperty("WidgetName", PROPERTY_TYPE_STRING),
	WidgetProperty("AnchorPoint", PROPERTY_TYPE_STRING),
	WidgetProperty("OnClickEvent", PROPERTY_TYPE_STRING),
	WidgetProperty("WidgetHidden", PROPERTY_TYPE_BOOLEAN),
	WidgetProperty("RelativeScale", PROPERTY_TYPE_FLOAT),
	WidgetProperty("RelativePosition", PROPERTY_TYPE_VECTOR2),
	WidgetProperty("RelativeRotation", PROPERTY_TYPE_FLOAT),
	WidgetProperty("HalfDimensions", PROPERTY_TYPE_VECTOR2),
	WidgetProperty("TextureName", PROPERTY_TYPE_STRING),
	WidgetProperty("TextureMinimums", PROPERTY_TYPE_VECTOR2),
	WidgetProperty("TextureMaximums", PROPERTY_TYPE_VECTOR2),
	WidgetProperty("TextureTint", PROPERTY_TYPE_COLOR),
	WidgetProperty("FontName", PROPERTY_TYPE_STRING),
	WidgetProperty("FontColor", PROPERTY_TYPE_COLOR),
	WidgetProperty("TextString", PROPERTY_TYPE_STRING),
	WidgetProperty("TextHeight", PROPERTY_TYPE_FLOAT),
	WidgetProperty("TextPaddingScale", PROPERTY_TYPE_FLOAT)
};



constexpr size_t NumberOfWidgetProperties()
{
	return (sizeof(s_WidgetProperties) / sizeof(s_WidgetProperties[0]));
}