#pragma once

#include "Engine/Renderer/UISystem/Widgets/BaseWidget.hpp"



class ButtonWidget : public BaseWidget
{
public:
	ButtonWidget(BaseWidget* parentWidget);
	~ButtonWidget();

	void Update(float) override;
	void Render() const override;

private:
	void RenderButtonText(ProportionalFont* buttonTextFont, const std::string& buttonTextString, const Vector2& buttonTextDimensions) const;
	void ResizeButtonWithText(AABB2& buttonBounds, const Vector2& buttonTextDimensions) const;
};