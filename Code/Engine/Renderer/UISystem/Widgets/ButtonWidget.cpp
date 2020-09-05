#include "Engine/Renderer/UISystem/Widgets/ButtonWidget.hpp"
#include "Engine/Renderer/UISystem/UISystem.hpp"



ButtonWidget::ButtonWidget(BaseWidget* parentWidget) :
BaseWidget(parentWidget)
{

}



ButtonWidget::~ButtonWidget()
{

}



void ButtonWidget::Update(float)
{
	if (IsWidgetHidden())
	{
		SetCurrentState(DISABLED_STATE);
		return;
	}

	if (IsCurrentState(DISABLED_STATE))
	{
		return;
	}
	
	float textHeight = GetStateSpecificWidgetProperty<float>("TextHeight");
	std::string textString = GetStateSpecificWidgetProperty<std::string>("TextString");

	std::string fontName = GetStateSpecificWidgetProperty<std::string>("FontName");
	ProportionalFont* buttonFont = ProportionalFont::CreateOrGetProportionalFont(fontName.c_str());
	float textWidth = ProportionalFont::CalculateTextWidth(textString.c_str(), textHeight, buttonFont);

	AABB2 buttonBounds = GetWidgetBounds();
	Vector2 buttonTextDimensions = Vector2(textWidth, textHeight);
	ResizeButtonWithText(buttonBounds, buttonTextDimensions);

	HandleClickingOnEvent(buttonBounds);
}



void ButtonWidget::Render() const
{
	if (IsWidgetHidden())
	{
		return;
	}
	
	float textHeight = GetStateSpecificWidgetProperty<float>("TextHeight");
	std::string textString = GetStateSpecificWidgetProperty<std::string>("TextString");

	std::string fontName = GetStateSpecificWidgetProperty<std::string>("FontName");
	ProportionalFont* buttonFont = ProportionalFont::CreateOrGetProportionalFont(fontName.c_str());
	float textWidth = ProportionalFont::CalculateTextWidth(textString.c_str(), textHeight, buttonFont);
	
	AABB2 buttonBounds = GetWidgetBounds();
	Vector2 buttonTextDimensions = Vector2(textWidth, textHeight);
	ResizeButtonWithText(buttonBounds, buttonTextDimensions);

	RenderWidget(buttonBounds);
	RenderButtonText(buttonFont, textString, buttonTextDimensions);
}



void ButtonWidget::RenderButtonText(ProportionalFont* buttonTextFont, const std::string& buttonTextString, const Vector2& buttonTextDimensions) const
{
	RGBA fontColor = GetStateSpecificWidgetProperty<RGBA>("FontColor");
	Vector2 textMinimums = GetWorldPosition() - (buttonTextDimensions * 0.5f);

	AdvancedRenderer::SingletonInstance()->Draw2DProportionalText(textMinimums, buttonTextString.c_str(), buttonTextDimensions.Y, buttonTextFont, fontColor);
}



void ButtonWidget::ResizeButtonWithText(AABB2& buttonBounds, const Vector2& buttonTextDimensions) const
{
	float textPaddingScale = GetStateSpecificWidgetProperty<float>("TextPaddingScale");
	float textPaddingAmount = textPaddingScale * buttonTextDimensions.Y;

	Vector2 buttonPosition = GetWorldPosition();

	float textWidthAmount = buttonTextDimensions.X * 0.5f + textPaddingAmount;
	float textHeightAmount = buttonTextDimensions.Y * 0.5f + textPaddingAmount;

	if (buttonBounds.minimums.X > buttonPosition.X - textWidthAmount)
	{
		buttonBounds.minimums.X = buttonPosition.X - textWidthAmount;
	}

	if (buttonBounds.maximums.X < buttonPosition.X + textWidthAmount)
	{
		buttonBounds.maximums.X = buttonPosition.X + textWidthAmount;
	}

	if (buttonBounds.minimums.Y > buttonPosition.Y - textHeightAmount)
	{
		buttonBounds.minimums.Y = buttonPosition.Y - textHeightAmount;
	}

	if (buttonBounds.maximums.Y < buttonPosition.Y + textHeightAmount)
	{
		buttonBounds.maximums.Y = buttonPosition.Y + textHeightAmount;
	}
}