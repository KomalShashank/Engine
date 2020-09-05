#include "Engine/Renderer/UISystem/Widgets/BaseWidget.hpp"
#include "Engine/Renderer/UISystem/UISystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/EventSystem/GlobalEventSystem.hpp"



BaseWidget::BaseWidget(BaseWidget* parentWidget) :
m_ParentWidget(parentWidget),
m_CurrentState(ACTIVE_STATE)
{
	
}



BaseWidget::~BaseWidget()
{
	
}



Vector2 BaseWidget::GetWorldPosition() const
{
	Vector2 relativePosition = GetStateSpecificWidgetProperty<Vector2>("RelativePosition");
	uint8_t anchorPoint = GetAnchorPoint();

	if (m_ParentWidget != nullptr)
	{
		return m_ParentWidget->GetWorldPosition() + relativePosition;
	}

	return UISystem::SingletonInstance()->GetOriginForAnchorPoint(anchorPoint) + relativePosition;
}



float BaseWidget::GetWorldRotation() const
{
	float relativeRotation = GetStateSpecificWidgetProperty<float>("RelativeRotation");

	if (m_ParentWidget != nullptr)
	{
		return m_ParentWidget->GetWorldRotation() + relativeRotation;
	}

	return relativeRotation;
}



float BaseWidget::GetWorldScale() const
{
	float relativeScale = GetStateSpecificWidgetProperty<float>("RelativeScale");

	if (m_ParentWidget != nullptr)
	{
		return m_ParentWidget->GetWorldScale() * relativeScale;
	}

	return relativeScale;
}



AABB2 BaseWidget::GetWidgetBounds() const
{
	Vector2 absolutePosition = GetWorldPosition();
	Vector2 halfDimensions = GetStateSpecificWidgetProperty<Vector2>("HalfDimensions");
	halfDimensions *= GetWorldScale();

	return AABB2(absolutePosition - halfDimensions, absolutePosition + halfDimensions);
}



uint8_t BaseWidget::GetAnchorPoint() const
{
	std::string anchorPoint = GetCommonWidgetProperty<std::string>("AnchorPoint");

	if (anchorPoint == "TopLeft")
	{
		return TOP_LEFT;
	}
	else if (anchorPoint == "TopCenter")
	{
		return TOP_CENTER;
	}
	else if (anchorPoint == "TopRight")
	{
		return TOP_RIGHT;
	}
	else if (anchorPoint == "MiddleLeft")
	{
		return MIDDLE_LEFT;
	}
	else if (anchorPoint == "MiddleCenter")
	{
		return MIDDLE_CENTER;
	}
	else if (anchorPoint == "MiddleRight")
	{
		return MIDDLE_RIGHT;
	}
	else if (anchorPoint == "BottomLeft")
	{
		return BOTTOM_LEFT;
	}
	else if (anchorPoint == "BottomCenter")
	{
		return BOTTOM_CENTER;
	}
	else if (anchorPoint == "BottomRight")
	{
		return BOTTOM_RIGHT;
	}

	return MIDDLE_CENTER;
}



Texture* BaseWidget::GetWidgetTexture() const
{
	std::string textureName = GetStateSpecificWidgetProperty<std::string>("TextureName");
	if (textureName == "None")
	{
		return nullptr;
	}

	textureName = "Data/Images/" + textureName + ".png";

	SamplerData samplerData = SamplerData(REPEAT_WRAP, REPEAT_WRAP, LINEAR_FILTER, NEAREST_FILTER);
	Texture* widgetTexture = Texture::CreateOrGetTexture(textureName.c_str(), samplerData);

	return widgetTexture;
}



bool BaseWidget::IsWidgetHidden() const
{
	bool widgetHidden = GetCommonWidgetProperty<bool>("WidgetHidden");
	
	if (m_ParentWidget != nullptr)
	{
		widgetHidden = m_ParentWidget->IsWidgetHidden() & widgetHidden;
	}

	return widgetHidden;
}



void BaseWidget::SetCurrentState(uint8_t currentState)
{
	m_CurrentState = currentState;
}



bool BaseWidget::IsCurrentState(uint8_t currentState) const
{
	return (m_CurrentState == currentState);
}



bool BaseWidget::IsMouseOverWidget(const AABB2& widgetBounds) const
{
	Vector2 mouseCursorPosition = InputSystem::SingletonInstance()->GetMouseCursorVirtualPosition(UISystem::SingletonInstance()->m_ViewDimensions);

	return (mouseCursorPosition.X > widgetBounds.minimums.X && mouseCursorPosition.X < widgetBounds.maximums.X &&
			mouseCursorPosition.Y > widgetBounds.minimums.Y && mouseCursorPosition.Y < widgetBounds.maximums.Y);
}



bool BaseWidget::IsMouseClickedOnWidget() const
{
	if (m_CurrentState == HIGHLIGHTED_STATE)
	{
		if (InputSystem::SingletonInstance()->MouseWasJustReleased(LEFT_MOUSE_BUTTON))
		{
			return true;
		}
	}

	return false;
}



bool BaseWidget::IsMouseHeldDownOnWidget() const
{
	if (m_CurrentState == HIGHLIGHTED_STATE)
	{
		if (InputSystem::SingletonInstance()->MouseButtonIsHeldDown(LEFT_MOUSE_BUTTON))
		{
			return true;
		}
	}

	return false;
}



void BaseWidget::SetWidgetPropertiesFromXML(NamedProperties& widgetProperties, const XMLAttribute& xmlAttribute)
{
	const char* widgetPropertyName = xmlAttribute.lpszName;
	const char* widgetPropertyValue = xmlAttribute.lpszValue;

	uint8_t widgetPropertyType = WidgetProperty::GetWidgetPropertyTypeForPropertyName(widgetPropertyName);
	
	switch (widgetPropertyType)
	{
	case PROPERTY_TYPE_UINT8:
	{
		uint8_t widgetProperty = ParseUInt8FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_UINT16:
	{
		uint16_t widgetProperty = ParseUInt16FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_UINT32:
	{
		uint32_t widgetProperty = ParseUInt32FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_UINT64:
	{
		uint64_t widgetProperty = ParseUInt64FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_FLOAT:
	{
		float widgetProperty = ParseFloatFromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_DOUBLE:
	{
		double widgetProperty = ParseDoubleFromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_BOOLEAN:
	{
		bool widgetProperty = ParseBooleanFromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
	break;

	case PROPERTY_TYPE_STRING:
	{
		std::string widgetProperty = widgetPropertyValue;
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_INTVECTOR2:
	{
		IntVector2 widgetProperty = ParseIntVector2FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_INTVECTOR3:
	{
		IntVector3 widgetProperty = ParseIntVector3FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_INTVECTOR4:
	{
		IntVector4 widgetProperty = ParseIntVector4FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_VECTOR2:
	{
		Vector2 widgetProperty = ParseVector2FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_VECTOR3:
	{
		Vector3 widgetProperty = ParseVector3FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_VECTOR4:
	{
		Vector4 widgetProperty = ParseVector4FromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_COLOR:
	{
		RGBA widgetProperty = ParseRGBAFromXML(widgetPropertyValue);
		widgetProperties.SetNamedProperty(widgetPropertyName, widgetProperty);
	}
		break;

	case PROPERTY_TYPE_TEXTURE:
		break;

	case PROPERTY_TYPE_FONT:
		break;

	case INVALID_PROPERTY_TYPE:
		break;

	default:
		break;
	}
}



void BaseWidget::HandleClickingOnEvent(const AABB2& widgetBounds)
{
	if (IsMouseOverWidget(widgetBounds))
	{
		m_CurrentState = HIGHLIGHTED_STATE;
	}
	else
	{
		m_CurrentState = ACTIVE_STATE;
	}

	if (IsMouseHeldDownOnWidget())
	{
		m_CurrentState = PRESSED_STATE;
	}

	if (IsMouseClickedOnWidget())
	{
		TriggerOnClickEvent();
	}
}



void BaseWidget::RenderWidget(const AABB2& widgetBounds) const
{
	const size_t NUMBER_OF_WIDGET_VERTICES = 4U;
	const size_t NUMBER_OF_WIDGET_INDICES = 6U;

	Vertex3D widgetVertices[NUMBER_OF_WIDGET_VERTICES];
	uint32_t widgetIndices[NUMBER_OF_WIDGET_INDICES] = { 0, 1, 2, 0, 2, 3 };

	RGBA widgetTint = GetStateSpecificWidgetProperty<RGBA>("TextureTint");
	Vector2 textureMinimums = GetStateSpecificWidgetProperty<Vector2>("TextureMinimums");
	Vector2 textureMaximums = GetStateSpecificWidgetProperty<Vector2>("TextureMaximums");

	Vertex3D widgetVertex;
	widgetVertex.m_Color = widgetTint;

	widgetVertex.m_Position = Vector3(widgetBounds.minimums.X, widgetBounds.minimums.Y, 0.0f);
	widgetVertex.m_TextureCoordinates = Vector2(textureMinimums.X, textureMaximums.Y);
	widgetVertices[0] = widgetVertex;

	widgetVertex.m_Position = Vector3(widgetBounds.maximums.X, widgetBounds.minimums.Y, 0.0f);
	widgetVertex.m_TextureCoordinates = Vector2(textureMaximums.X, textureMaximums.Y);
	widgetVertices[1] = widgetVertex;

	widgetVertex.m_Position = Vector3(widgetBounds.maximums.X, widgetBounds.maximums.Y, 0.0f);
	widgetVertex.m_TextureCoordinates = Vector2(textureMaximums.X, textureMinimums.Y);
	widgetVertices[2] = widgetVertex;

	widgetVertex.m_Position = Vector3(widgetBounds.minimums.X, widgetBounds.maximums.Y, 0.0f);
	widgetVertex.m_TextureCoordinates = Vector2(textureMinimums.X, textureMinimums.Y);
	widgetVertices[3] = widgetVertex;

	Mesh widgetMesh;
	widgetMesh.WriteToMesh(&widgetVertices[0], &widgetIndices[0], NUMBER_OF_WIDGET_VERTICES, NUMBER_OF_WIDGET_INDICES);

	Material* widgetMaterial = nullptr;
	Texture* widgetTexture = GetWidgetTexture();
	if (widgetTexture != nullptr)
	{
		widgetMaterial = UISystem::SingletonInstance()->GetDefaultMaterial();
		widgetMaterial->SetDiffuseTexture(widgetTexture);
	}

	AdvancedRenderer::SingletonInstance()->DrawPolygonMesh(&widgetMesh, NUMBER_OF_WIDGET_VERTICES, NUMBER_OF_WIDGET_INDICES, widgetMaterial);
}



void BaseWidget::TriggerOnClickEvent() const
{
	std::string eventName = GetCommonWidgetProperty<std::string>("OnClickEvent");
	GlobalEventSystem::SingletonInstance()->TriggerEvent(eventName.c_str());
}