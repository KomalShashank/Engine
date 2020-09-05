#pragma once

#include <vector>

#include "Engine/Renderer/UISystem/Widgets/BaseWidget.hpp"
#include "Engine/Renderer/UISystem/WidgetProperty.hpp"



class Clock;



class UISystem
{
private:
	UISystem(const Vector2& viewDimensions);
	~UISystem();

	void InitializeFromXML();
	void LoadWidgetFromXMLNode(const XMLNode& widgetNode, BaseWidget* parentWidget);
	BaseWidget* CreateWidgetByType(const char* widgetTypeName, BaseWidget* parentWidget);

	void LoadStateSpecificAttributes(const XMLNode& widgetNode, const char* stateName, BaseWidget* currentWidget) const;
	uint8_t GetStateForStateName(const char* stateName) const;

public:
	static void InitializeUISystem(const Vector2& viewDimensions);
	static void UninitializeUISystem();
	static UISystem* SingletonInstance();

	void Update();
	void Render() const;

	Vector2 GetOriginForAnchorPoint(uint8_t anchorPoint);
	Material* GetDefaultMaterial() const;

	BaseWidget* GetWidgetByName(const char* widgetName) const;

public:
	Clock* m_UISystemClock;
	Vector2 m_ViewDimensions;

	std::vector<BaseWidget*> m_AllWidgets;

private:
	Material* m_DefaultMaterial;
};