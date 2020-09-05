#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"

#pragma comment (lib, "xinput9_1_0")



InputSystem* g_InputSystem = nullptr;
XINPUT_STATE m_XBOXControllerState[NUMBER_OF_CONTROLLERS];



InputSystem::InputSystem(void* windowHandle) :
m_WindowHandle(windowHandle)
{
	InitializeKeyDownStates();
	InitializeMouseClickedStates();

	for (int controllerIndex = 0; controllerIndex < NUMBER_OF_CONTROLLERS; ++controllerIndex)
	{
		InitializeController(controllerIndex);
	}
}



void InputSystem::InitializeInputSystem(void* windowHandle)
{
	if (g_InputSystem == nullptr)
	{
		g_InputSystem = new InputSystem(windowHandle);
	}
}



void InputSystem::UninitializeInputSystem()
{
	if (g_InputSystem != nullptr)
	{
		delete g_InputSystem;
		g_InputSystem = nullptr;
	}
}



InputSystem* InputSystem::SingletonInstance()
{
	ASSERT_OR_DIE(g_InputSystem != nullptr, "Input System instance does not exist.");

	return g_InputSystem;
}



void InputSystem::UpdateInputSystem()
{
	ResetKeyChangedState();
	ResetMouseStates();

	for (int controllerIndex = 0; controllerIndex < NUMBER_OF_CONTROLLERS; ++controllerIndex)
	{
		UpdateControllerStatus(controllerIndex);
	}
}



void InputSystem::InitializeKeyDownStates()
{
	for (int keyIndex = 0; keyIndex < NUMBER_OF_KEYS; ++keyIndex)
	{
		m_IsKeyDown[keyIndex].isDown = false;
		m_IsKeyDown[keyIndex].justChanged = false;
	}
}



void InputSystem::ResetKeyChangedState()
{
	for (int keyIndex = 0; keyIndex < NUMBER_OF_KEYS; ++keyIndex)
	{
		m_IsKeyDown[keyIndex].justChanged = false;
	}
}



void InputSystem::SetKeyDownState(unsigned char key, bool isDownNow)
{
	m_IsKeyDown[key].justChanged = (m_IsKeyDown[key].isDown != isDownNow);
	if (m_IsKeyDown[key].justChanged)
	{
		m_IsKeyDown[key].isDown = isDownNow;
	}
}



bool InputSystem::KeyWasJustPressed(unsigned char key)
{
	return m_IsKeyDown[key].isDown && m_IsKeyDown[key].justChanged;
}



bool InputSystem::KeyIsHeldDown(unsigned char key)
{
	return m_IsKeyDown[key].isDown;
}



void InputSystem::InitializeMouseClickedStates()
{
	for (int mouseButtonIndex = 0; mouseButtonIndex < NUMBER_OF_MOUSE_BUTTONS; ++mouseButtonIndex)
	{
		m_IsClickedDown[mouseButtonIndex].isClicked = false;
		m_IsClickedDown[mouseButtonIndex].justChanged = false;
	}
}



void InputSystem::ResetMouseStates()
{
	for (int mouseButtonIndex = 0; mouseButtonIndex < NUMBER_OF_MOUSE_BUTTONS; ++mouseButtonIndex)
	{
		m_IsClickedDown[mouseButtonIndex].justChanged = false;
	}

	SetMouseWheelDirection(0);
}



void InputSystem::SetMouseClickedState(const MouseButton& mouseButton, bool isClickedNow)
{
	m_IsClickedDown[mouseButton].justChanged = (m_IsClickedDown[mouseButton].isClicked != isClickedNow);
	if (m_IsClickedDown[mouseButton].justChanged)
	{
		m_IsClickedDown[mouseButton].isClicked = isClickedNow;
	}
}



bool InputSystem::MouseWasJustClicked(const MouseButton& mouseButton)
{
	return m_IsClickedDown[mouseButton].isClicked && m_IsClickedDown[mouseButton].justChanged;
}



bool InputSystem::MouseWasJustReleased(const MouseButton& mouseButton)
{
	return !m_IsClickedDown[mouseButton].isClicked && m_IsClickedDown[mouseButton].justChanged;
}



bool InputSystem::MouseButtonIsHeldDown(const MouseButton& mouseButton)
{
	return m_IsClickedDown[mouseButton].isClicked;
}



void InputSystem::SetMouseCursorScreenPosition(const IntVector2& cursorPosition)
{
	SetCursorPos(cursorPosition.X, cursorPosition.Y);
}



IntVector2 InputSystem::GetMouseCursorScreenPosition()
{
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);

	return IntVector2(cursorPosition.x, cursorPosition.y);
}



void InputSystem::SetMouseCursorVirtualPosition(const Vector2& virtualPosition, const Vector2& viewDimensions)
{
	RECT windowRectangle;
	GetClientRect((HWND)m_WindowHandle, &windowRectangle);

	POINT cursorPosition;
	cursorPosition.x = (int)(virtualPosition.X / viewDimensions.X * (float)windowRectangle.right);
	cursorPosition.y = (int)((1.0f - (virtualPosition.Y / viewDimensions.Y)) * (float)windowRectangle.bottom);
	
	ClientToScreen((HWND)m_WindowHandle, &cursorPosition);
	SetCursorPos(cursorPosition.x, cursorPosition.y);
}



Vector2 InputSystem::GetMouseCursorVirtualPosition(const Vector2& viewDimensions)
{
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);
	ScreenToClient((HWND)m_WindowHandle, &cursorPosition);

	RECT windowRectangle;
	GetClientRect((HWND)m_WindowHandle, &windowRectangle);

	Vector2 virtualPosition;
	virtualPosition.X = ((float)cursorPosition.x / (float)windowRectangle.right) * viewDimensions.X;
	virtualPosition.Y = (1.0f - ((float)cursorPosition.y / (float)windowRectangle.bottom)) * viewDimensions.Y;

	return virtualPosition;
}



void InputSystem::ShowMouseCursor()
{
	ShowCursor(true);
}



void InputSystem::HideMouseCursor()
{
	ShowCursor(false);
}



int InputSystem::GetMouseWheelDirection()
{
	return m_MouseWheelDirection;
}



void InputSystem::SetMouseWheelDirection(int mouseWheelDirection)
{
	m_MouseWheelDirection = mouseWheelDirection;
}



void InputSystem::InitializeController(int controllerNumber)
{
	memset(&m_XBOXControllerState[controllerNumber], 0, sizeof(m_XBOXControllerState[controllerNumber]));
	m_ControllerStatus[controllerNumber] = XInputGetState(controllerNumber, &m_XBOXControllerState[controllerNumber]);
}



void InputSystem::UpdateControllerStatus(int controllerNumber)
{
	memset(&m_XBOXControllerState[controllerNumber], 0, sizeof(m_XBOXControllerState[controllerNumber]));
	m_ControllerStatus[controllerNumber] = XInputGetState(controllerNumber, &m_XBOXControllerState[controllerNumber]);

	if (m_ControllerStatus[controllerNumber] == ERROR_SUCCESS)
	{
		UpdateButtonState(controllerNumber, D_PAD_UP, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		UpdateButtonState(controllerNumber, D_PAD_DOWN, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
		UpdateButtonState(controllerNumber, D_PAD_LEFT, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButtonState(controllerNumber, D_PAD_RIGHT, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);

		UpdateButtonState(controllerNumber, A_BUTTON, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_A);
		UpdateButtonState(controllerNumber, B_BUTTON, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_B);
		UpdateButtonState(controllerNumber, X_BUTTON, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_X);
		UpdateButtonState(controllerNumber, Y_BUTTON, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_Y);

		UpdateButtonState(controllerNumber, LEFT_BUMPER, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButtonState(controllerNumber, RIGHT_BUMPER, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);

		UpdateButtonState(controllerNumber, START_BUTTON, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_START);
		UpdateButtonState(controllerNumber, BACK_BUTTON, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_BACK);

		UpdateButtonState(controllerNumber, LEFT_STICK_BUTTON, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButtonState(controllerNumber, RIGHT_STICK_BUTTON, m_XBOXControllerState[controllerNumber].Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
	}
}



void InputSystem::UpdateButtonState(int controllerNumber, const XBOXButton& pressedButton, unsigned short buttonBitFlags, unsigned short buttonFlag)
{
	ButtonState& currentButtonState = m_Buttons[controllerNumber][pressedButton];
	bool wasDownBefore = currentButtonState.isPressed;
	bool isDownNow = (buttonFlag & buttonBitFlags) != 0;
	m_Buttons[controllerNumber][pressedButton].isPressed = isDownNow;
	m_Buttons[controllerNumber][pressedButton].justChanged = (wasDownBefore != isDownNow);
}



bool InputSystem::ButtonWasJustPressed(int controllerNumber, const XBOXButton& pressedButton)
{
	if (m_ControllerStatus[controllerNumber] == ERROR_SUCCESS)
	{
		return m_Buttons[controllerNumber][pressedButton].isPressed && m_Buttons[controllerNumber][pressedButton].justChanged;
	}

	return false;
}



bool InputSystem::ButtonIsHeldDown(int controllerNumber, const XBOXButton& pressedButton)
{
	if (m_ControllerStatus[controllerNumber] == ERROR_SUCCESS)
	{
		return m_Buttons[controllerNumber][pressedButton].isPressed;
	}

	return false;
}



void InputSystem::Vibrate(int controllerNumber, int leftMotorSpeed, int rightMotorSpeed)
{
	XINPUT_VIBRATION controllerVibration;
	ZeroMemory(&controllerVibration, sizeof(XINPUT_VIBRATION));
	controllerVibration.wLeftMotorSpeed = static_cast<WORD>(leftMotorSpeed);
	controllerVibration.wRightMotorSpeed = static_cast<WORD>(rightMotorSpeed);
	XInputSetState(controllerNumber, &controllerVibration);
}



unsigned char InputSystem::GetInputTriggerPosition(int controllerNumber, const XBOXTrigger& pressedTrigger)
{
	if (m_ControllerStatus[controllerNumber] == ERROR_SUCCESS)
	{
		if (pressedTrigger == LEFT_TRIGGER)
		{
			return m_XBOXControllerState[controllerNumber].Gamepad.bLeftTrigger;
		}
		else if (pressedTrigger == RIGHT_TRIGGER)
		{
			return m_XBOXControllerState[controllerNumber].Gamepad.bRightTrigger;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}



float InputSystem::TriggerPosition(int controllerNumber, const XBOXTrigger& pressedTrigger)
{
	const float MINIMUM_THRESHOLD = 30.0f;
	const float MAXIMUM_THRESHOLD = 255.0f;

	float inputTriggerPosition = static_cast<float>(GetInputTriggerPosition(controllerNumber,pressedTrigger));
	float mappedTriggerPosition = RangeMap(inputTriggerPosition, MINIMUM_THRESHOLD, MAXIMUM_THRESHOLD, MAPPED_MINIMUM_THRESHOLD, MAPPED_MAXIMUM_THRESHOLD);
	mappedTriggerPosition = ClampFloat(mappedTriggerPosition, MAPPED_MINIMUM_THRESHOLD, MAPPED_MAXIMUM_THRESHOLD);

	return mappedTriggerPosition;
}



Vector2 InputSystem::GetInputAnalogStickPosition(int controllerNumber, const XBOXStick& movedStick)
{
	if (m_ControllerStatus[controllerNumber] == ERROR_SUCCESS)
	{
		if (movedStick == LEFT_STICK)
		{
			float X = static_cast<float>(m_XBOXControllerState[controllerNumber].Gamepad.sThumbLX);
			float Y = static_cast<float>(m_XBOXControllerState[controllerNumber].Gamepad.sThumbLY);

			return Vector2(X, Y);
		}
		else if (movedStick == RIGHT_STICK)
		{
			float X = static_cast<float>(m_XBOXControllerState[controllerNumber].Gamepad.sThumbRX);
			float Y = static_cast<float>(m_XBOXControllerState[controllerNumber].Gamepad.sThumbRY);

			return Vector2(X, Y);
		}
		else
		{
			return Vector2(0.0f, 0.0f);
		}
	}
	else
	{
		return Vector2(0.0f, 0.0f);
	}
}



Vector2 InputSystem::AnalogStickPolarPosition(int controllerNumber, const XBOXStick& movedStick)
{
	const float maximumXY = 32768.0f;
	const float MINIMUM_THRESHOLD = 8270.0f / maximumXY;
	const float MAXIMUM_THRESHOLD = 32000.0f / maximumXY;

	float positionX = GetInputAnalogStickPosition(controllerNumber,movedStick).X / maximumXY;
	float positionY = GetInputAnalogStickPosition(controllerNumber,movedStick).Y / maximumXY;
	Vector2 inputPosition = Vector2(positionX, positionY);

	float inputRadius = ConvertToPolar(inputPosition).X;
	float inputAngle = ConvertToPolar(inputPosition).Y;
	float mappedRadius = RangeMap(inputRadius, MINIMUM_THRESHOLD, MAXIMUM_THRESHOLD, MAPPED_MINIMUM_THRESHOLD, MAPPED_MAXIMUM_THRESHOLD);
	mappedRadius = ClampFloat(mappedRadius, MAPPED_MINIMUM_THRESHOLD, MAPPED_MAXIMUM_THRESHOLD);

	return Vector2(mappedRadius, inputAngle);
}



Vector2 InputSystem::AnalogStickCartesianPosition(int controllerNumber, const XBOXStick& movedStick)
{
	Vector2 analogStickPolarPosition = AnalogStickPolarPosition(controllerNumber, movedStick);
	Vector2 analogStickCartesianPosition = ConvertToCartesian(analogStickPolarPosition);

	return analogStickCartesianPosition;
}