#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



#define BACKSPACE_KEY	0x08
#define TAB_KEY			0x09

#define ENTER_KEY		0x0D

#define SHIFT_KEY		0x10
#define CTRL_KEY		0x11
#define ALT_KEY			0x12
#define PAUSE_KEY		0x13
#define CAPS_LOCK_KEY	0x14

#define ESCAPE_KEY		0x1B

#define SPACEBAR_KEY	0x20
#define PAGE_UP_KEY		0x21
#define PAGE_DOWN_KEY	0x22
#define END_KEY			0x23
#define HOME_KEY		0x24
#define LEFT_ARROW_KEY	0x25
#define UP_ARROW_KEY	0x26
#define RIGHT_ARROW_KEY	0x27
#define DOWN_ARROW_KEY	0x28

#define INSERT_KEY		0x2D
#define DELETE_KEY		0x2E

#define NUMPAD_0_KEY	0x60
#define NUMPAD_1_KEY	0x61
#define NUMPAD_2_KEY	0x62
#define NUMPAD_3_KEY	0x63
#define NUMPAD_4_KEY	0x64
#define NUMPAD_5_KEY	0x65
#define NUMPAD_6_KEY	0x66
#define NUMPAD_7_KEY	0x67
#define NUMPAD_8_KEY	0x68
#define NUMPAD_9_KEY	0x69

#define SEMICOLON_KEY	0xBA
#define PLUS_KEY		0xBB
#define COMMA_KEY		0xBC
#define MINUS_KEY		0xBD
#define FULLSTOP_KEY	0xBE
#define SLASH_KEY		0xBF



const int NUMBER_OF_KEYS = 256;
const int NUMBER_OF_CONTROLLERS = 4;



struct KeyState
{
	bool isDown;
	bool justChanged;
};



enum MouseButton
{
	LEFT_MOUSE_BUTTON,
	MIDDLE_MOUSE_BUTTON,
	RIGHT_MOUSE_BUTTON,
	NUMBER_OF_MOUSE_BUTTONS
};



struct MouseState
{
	bool isClicked;
	bool justChanged;
};



const float MAPPED_MINIMUM_THRESHOLD = 0.0f;
const float MAPPED_MAXIMUM_THRESHOLD = 1.0f;



enum XBOXButton
{
	D_PAD_UP, D_PAD_DOWN, D_PAD_LEFT, D_PAD_RIGHT,
	A_BUTTON, B_BUTTON, X_BUTTON, Y_BUTTON,
	LEFT_BUMPER, RIGHT_BUMPER, START_BUTTON, BACK_BUTTON,
	LEFT_STICK_BUTTON, RIGHT_STICK_BUTTON,
	NUMBER_OF_BUTTONS
};



enum XBOXTrigger
{
	LEFT_TRIGGER,
	RIGHT_TRIGGER
};



enum XBOXStick
{
	LEFT_STICK,
	RIGHT_STICK
};



struct ButtonState
{
	bool isPressed;
	bool justChanged;
};



class InputSystem
{
private:
	InputSystem(void* windowHandle);

public:
	static void InitializeInputSystem(void* windowHandle);
	static void UninitializeInputSystem();

	static InputSystem* SingletonInstance();

	void UpdateInputSystem();

	void InitializeKeyDownStates();
	void ResetKeyChangedState();
	void SetKeyDownState(unsigned char key, bool isDownNow);
	bool KeyWasJustPressed(unsigned char key);
	bool KeyIsHeldDown(unsigned char key);

	void InitializeMouseClickedStates();
	void ResetMouseStates();
	void SetMouseClickedState(const MouseButton& mouseButton, bool isClickedNow);
	bool MouseWasJustClicked(const MouseButton& mouseButton);
	bool MouseWasJustReleased(const MouseButton& mouseButton);
	bool MouseButtonIsHeldDown(const MouseButton& mouseButton);

	void SetMouseCursorScreenPosition(const IntVector2& cursorPosition);
	IntVector2 GetMouseCursorScreenPosition();

	void SetMouseCursorVirtualPosition(const Vector2& virtualPosition, const Vector2& viewDimensions);
	Vector2 GetMouseCursorVirtualPosition(const Vector2& viewDimensions);

	void ShowMouseCursor();
	void HideMouseCursor();

	int GetMouseWheelDirection();
	void SetMouseWheelDirection(int mouseWheelDirection);
	
	void InitializeController(int controllerNumber);
	void UpdateControllerStatus(int controllerNumber);
	void UpdateButtonState(int controllerNumber, const XBOXButton& pressedButton, unsigned short buttonBitFlags, unsigned short buttonFlag);
	bool ButtonWasJustPressed(int controllerNumber, const XBOXButton& pressedButton);
	bool ButtonIsHeldDown(int controllerNumber, const XBOXButton& pressedButton);

	void Vibrate(int controllerNumber, int leftMotorSpeed, int rightMotorSpeed);
	
	unsigned char GetInputTriggerPosition(int controllerNumber, const XBOXTrigger& pressedTrigger);
	float TriggerPosition(int controllerNumber, const XBOXTrigger& pressedTrigger);
	
	Vector2 GetInputAnalogStickPosition(int controllerNumber, const XBOXStick& movedStick);
	Vector2 AnalogStickPolarPosition(int controllerNumber, const XBOXStick& movedStick);
	Vector2 AnalogStickCartesianPosition(int controllerNumber, const XBOXStick& movedStick);

private:
	void* m_WindowHandle;

	KeyState m_IsKeyDown[NUMBER_OF_KEYS];
	MouseState m_IsClickedDown[NUMBER_OF_MOUSE_BUTTONS];

	int m_MouseWheelDirection;

	unsigned long m_ControllerStatus[NUMBER_OF_CONTROLLERS];
	ButtonState m_Buttons[NUMBER_OF_CONTROLLERS][NUMBER_OF_BUTTONS];
};