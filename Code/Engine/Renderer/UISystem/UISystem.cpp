#include "Engine/Renderer/UISystem/UISystem.hpp"
#include "Engine/Renderer/UISystem/Widgets/ButtonWidget.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/FileUtilities/FileUtilities.hpp"
#include "Engine/Time/Clock.hpp"



UISystem* g_UISystem = nullptr;



UISystem::UISystem(const Vector2& viewDimensions) :
m_ViewDimensions(viewDimensions)
{
	m_UISystemClock = new Clock();
	m_DefaultMaterial = new Material("Data/Shaders/DefaultShader.vert", "Data/Shaders/DefaultShader.frag");

	InitializeFromXML();
}



UISystem::~UISystem()
{
	delete m_UISystemClock;
	delete m_DefaultMaterial;
}



void UISystem::InitializeFromXML()
{
	std::vector<std::string> foundFiles;
	g_FileUtilities->EnumerateFilesInDirectory("Data/XML", "*.UI.xml", foundFiles);
	
	for(const std::string& currentFileName : foundFiles)
	{
		XMLNode xmlNode = XMLNode::openFileHelper(currentFileName.c_str());
		XMLNode rootNode = xmlNode.getChildNode();
		if (rootNode.isEmpty())
		{
			continue;
		}

		int widgetIndex = 0;
		while (true)
		{
			XMLNode widgetNode = rootNode.getChildNode("UIWidget", &widgetIndex);
			if (widgetNode.isEmpty())
			{
				break;
			}

			LoadWidgetFromXMLNode(widgetNode, nullptr);
		}
	}
}



void UISystem::LoadWidgetFromXMLNode(const XMLNode& widgetNode, BaseWidget* parentWidget)
{
	const char* widgetType = widgetNode.getAttribute("WidgetType");
	BaseWidget* currentWidget = CreateWidgetByType(widgetType, parentWidget);

	if (currentWidget == nullptr)
	{
		return;
	}

	uint8_t widgetCurrentState = GetStateForStateName(widgetNode.getAttribute("CurrentState"));
	currentWidget->SetCurrentState(widgetCurrentState);

	int numberOfCommonAttributes = widgetNode.nAttribute();
	for (int commonAttributeIndex = 2; commonAttributeIndex < numberOfCommonAttributes; ++commonAttributeIndex)
	{
		XMLAttribute currentCommonAttribute = widgetNode.getAttribute(commonAttributeIndex);
		currentWidget->SetWidgetPropertiesFromXML(currentWidget->m_CommonProperties, currentCommonAttribute);
	}

	LoadStateSpecificAttributes(widgetNode, "ActiveState", currentWidget);
	LoadStateSpecificAttributes(widgetNode, "HighlightedState", currentWidget);
	LoadStateSpecificAttributes(widgetNode, "PressedState", currentWidget);
	LoadStateSpecificAttributes(widgetNode, "DisabledState", currentWidget);

	m_AllWidgets.push_back(currentWidget);

	int numberOfChildWidgets = widgetNode.nChildNode("UIWidget");
	if (numberOfChildWidgets > 0)
	{
		int widgetIndex = 0;
		while (true)
		{
			XMLNode childWidgetNode = widgetNode.getChildNode("UIWidget", &widgetIndex);
			if (childWidgetNode.isEmpty())
			{
				break;
			}

			LoadWidgetFromXMLNode(childWidgetNode, currentWidget);
		}
	}
}



BaseWidget* UISystem::CreateWidgetByType(const char* widgetTypeName, BaseWidget* parentWidget)
{
	if (strcmp(widgetTypeName, "Button") == 0)
	{
		return new ButtonWidget(parentWidget);
	}

	return nullptr;
}



void UISystem::LoadStateSpecificAttributes(const XMLNode& widgetNode, const char* stateName, BaseWidget* currentWidget) const
{
	XMLNode stateNode = widgetNode.getChildNode(stateName);
	uint8_t currentState = GetStateForStateName(stateName);
	
	int numberOfChildNodes = stateNode.nChildNode();
	for (int childNodeIndex = 0; childNodeIndex < numberOfChildNodes; ++childNodeIndex)
	{
		XMLNode currentChildNode = stateNode.getChildNode(childNodeIndex);
		
		int numberOfAttributes = currentChildNode.nAttribute();
		for (int attributeIndex = 0; attributeIndex < numberOfAttributes; ++attributeIndex)
		{
			XMLAttribute currentAttribute = currentChildNode.getAttribute(attributeIndex);
			currentWidget->SetWidgetPropertiesFromXML(currentWidget->m_StateSpecificProperties[currentState], currentAttribute);
		}
	}
}



uint8_t UISystem::GetStateForStateName(const char* stateName) const
{
	if (strcmp(stateName, "ActiveState") == 0)
	{
		return ACTIVE_STATE;
	}
	else if (strcmp(stateName, "HighlightedState") == 0)
	{
		return HIGHLIGHTED_STATE;
	}
	else if (strcmp(stateName, "PressedState") == 0)
	{
		return PRESSED_STATE;
	}
	else if (strcmp(stateName, "DisabledState") == 0)
	{
		return DISABLED_STATE;
	}

	return NUMBER_OF_WIDGET_STATES;
}



void UISystem::InitializeUISystem(const Vector2& viewDimensions)
{
	if (g_UISystem == nullptr)
	{
		g_UISystem = new UISystem(viewDimensions);
	}
}



void UISystem::UninitializeUISystem()
{
	if (g_UISystem != nullptr)
	{
		delete g_UISystem;
		g_UISystem = nullptr;
	}
}



UISystem* UISystem::SingletonInstance()
{
	ASSERT_OR_DIE(g_UISystem != nullptr, "A UI System instance does not exist.");

	return g_UISystem;
}



void UISystem::Update()
{
	float deltaTimeInSeconds = m_UISystemClock->GetDeltaTimeFloat();
	
	for (BaseWidget* currentWidget : m_AllWidgets)
	{
		if (currentWidget != nullptr)
		{
			currentWidget->Update(deltaTimeInSeconds);
		}
	}
}



void UISystem::Render() const
{
	for (BaseWidget* currentWidget : m_AllWidgets)
	{
		if (currentWidget != nullptr)
		{
			currentWidget->Render();
		}
	}
}



Vector2 UISystem::GetOriginForAnchorPoint(uint8_t anchorPoint)
{
	float viewWidth = m_ViewDimensions.X;
	float viewHeight = m_ViewDimensions.Y;

	switch (anchorPoint)
	{
	case TOP_LEFT:
		return Vector2(0.0f, viewHeight);

	case TOP_CENTER:
		return Vector2(viewWidth * 0.5f, viewHeight);

	case TOP_RIGHT:
		return Vector2(viewWidth, viewHeight);

	case MIDDLE_LEFT:
		return Vector2(0.0f, viewHeight * 0.5f);

	case MIDDLE_CENTER:
		return Vector2(viewWidth * 0.5f, viewHeight * 0.5f);

	case MIDDLE_RIGHT:
		return Vector2(viewWidth, viewHeight * 0.5f);

	case BOTTOM_LEFT:
		return Vector2(0.0f, 0.0f);

	case BOTTOM_CENTER:
		return Vector2(viewWidth * 0.5f, 0.0f);

	case BOTTOM_RIGHT:
		return Vector2(viewWidth, 0.0f);

	default:
		return Vector2();
	}
}



Material* UISystem::GetDefaultMaterial() const
{
	return m_DefaultMaterial;
}



BaseWidget* UISystem::GetWidgetByName(const char* widgetName) const
{
	for (BaseWidget* currentWidget : m_AllWidgets)
	{
		if (currentWidget != nullptr)
		{
			std::string currentWidgetName = currentWidget->GetCommonWidgetProperty<std::string>("WidgetName");
			if (strcmp(currentWidgetName.c_str(), widgetName) == 0)
			{
				return currentWidget;
			}
		}
	}

	return nullptr;
}