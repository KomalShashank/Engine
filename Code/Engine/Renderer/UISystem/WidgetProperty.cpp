#include <string.h>

#include "Engine/Renderer/UISystem/WidgetProperty.hpp"



const char* WidgetProperty::GetWidgetPropertyName() const
{
	return m_WidgetPropertyName;
}



uint8_t WidgetProperty::GetWidgetPropertyType() const
{
	return m_WidgetPropertyType;
}



uint8_t WidgetProperty::GetWidgetPropertyTypeForPropertyName(const char* widgetPropertyName)
{
	size_t numberOfWidgetProperties = NumberOfWidgetProperties();
	for (size_t propertyIndex = 0; propertyIndex < numberOfWidgetProperties; ++propertyIndex)
	{
		const char* currentWidgetPropertyName = s_WidgetProperties[propertyIndex].GetWidgetPropertyName();
		if (strcmp(currentWidgetPropertyName, widgetPropertyName) == 0)
		{
			return s_WidgetProperties[propertyIndex].GetWidgetPropertyType();
		}
	}

	return INVALID_PROPERTY_TYPE;
}