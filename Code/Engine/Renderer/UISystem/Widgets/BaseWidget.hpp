#pragma once

#include <stdint.h>
#include <map>

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/VectorMath/4D/Vector4.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/Math/VectorMath/3D/IntVector3.hpp"
#include "Engine/Math/VectorMath/4D/IntVector4.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"

#include "Engine/Math/VectorMath/2D/AABB2.hpp"
#include "Engine/EventSystem/NamedProperties.hpp"
#include "Engine/Tools/XMLUtilities.hpp"



enum AnchorPoint : uint8_t
{
	TOP_LEFT,
	TOP_CENTER,
	TOP_RIGHT,
	MIDDLE_LEFT,
	MIDDLE_CENTER,
	MIDDLE_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT
};



enum WidgetState : uint8_t
{
	ACTIVE_STATE,
	HIGHLIGHTED_STATE,
	PRESSED_STATE,
	DISABLED_STATE,
	NUMBER_OF_WIDGET_STATES
};



class BaseWidget
{
public:
	BaseWidget(BaseWidget* parentWidget);
	virtual ~BaseWidget();

	virtual void Update(float) = 0;
	virtual void Render() const = 0;

	Vector2 GetWorldPosition() const;
	float GetWorldRotation() const;
	float GetWorldScale() const;

	AABB2 GetWidgetBounds() const;
	uint8_t GetAnchorPoint() const;
	Texture* GetWidgetTexture() const;
	bool IsWidgetHidden() const;

	void SetCurrentState(uint8_t currentState);
	bool IsCurrentState(uint8_t currentState) const;

	bool IsMouseOverWidget(const AABB2& widgetBounds) const;
	bool IsMouseClickedOnWidget() const;
	bool IsMouseHeldDownOnWidget() const;

	void SetWidgetPropertiesFromXML(NamedProperties& widgetProperties, const XMLAttribute& xmlAttribute);

	template <typename data_type>
	data_type GetStateSpecificWidgetProperty(const char* widgetPropertyName) const
	{
		data_type widgetProperty;
		m_StateSpecificProperties[m_CurrentState].GetNamedProperty(widgetPropertyName, widgetProperty);

		return widgetProperty;
	}

	template <typename data_type>
	data_type GetCommonWidgetProperty(const char* widgetPropertyName) const
	{
		data_type widgetProperty;
		m_CommonProperties.GetNamedProperty(widgetPropertyName, widgetProperty);

		return widgetProperty;
	}

protected:
	virtual void HandleClickingOnEvent(const AABB2& widgetBounds);
	virtual void RenderWidget(const AABB2& widgetBounds) const;
	virtual void TriggerOnClickEvent() const;

private:
	BaseWidget* m_ParentWidget;
	uint8_t m_CurrentState;

public:
	NamedProperties m_CommonProperties;
	NamedProperties m_StateSpecificProperties[NUMBER_OF_WIDGET_STATES];
};