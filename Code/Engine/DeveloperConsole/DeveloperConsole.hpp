#pragma once

#include "Engine/Math/VectorMath/2D/AABB2.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"
#include "Engine/Renderer/BitmapFonts/BitmapFont.hpp"
#include "Engine/DeveloperConsole/Command.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"
#include "Engine/DebugTools/MemoryAnalytics/UntrackedAllocator.hpp"

#include <vector>
#include <map>



typedef void (ConsoleCommandCallBack)(Command&);



struct ConsoleLine
{
public:
	ConsoleLine();
	ConsoleLine(const std::string& consoleLineText, const RGBA& consoleLineColor = RGBA::WHITE);

public:
	std::string m_ConsoleLineText;
	RGBA m_ConsoleLineColor;
};



class DeveloperConsole
{
public:
	DeveloperConsole(const Vector2& viewDimensions);

	void Update(float deltaTimeInSeconds);
	void Render() const;

	float CalculateTextWidth(const std::string& asciiText, float cellHeight, const ProportionalFont* font = nullptr) const;
	bool IsOpen() const;

	void ToggleConsole();
	void CloseConsole();
	void UpdateCursor(float deltaTimeInSeconds);
	void ProcessInputForConsole();
	void ScrollThroughConsole();

	void ResetNewestConsoleLineMinimums();

	void GetLiveInput(unsigned char inputCharacter);

	void PrintCurrentLiveInput() const;

	void PrintLineToConsole(const Vector2& startingLetterMinimums, float cellHeight, const ConsoleLine& consoleLine) const;
	void PrintAllConsoleLines() const;

	static void AddNewConsoleLine(const ConsoleLine& newConsoleLine);
	static void ClearAllConsoleLines();
	bool ConsoleIsEmpty();

	static void RegisterCommands(const char* commandName, const char* commandDescription, ConsoleCommandCallBack* commandCallBack);
	void RunConsoleCommand(const char* consoleCommand);

	static std::vector<std::string, UntrackedAllocator<std::string>>& GetAllCommandDescriptions();

private:
	bool m_ConsoleIsOpen;
	AABB2 m_ScreenBounds;

	AABB2 m_PromptBoxBounds;
	AABB2 m_SurroundingBoxBounds;

	float m_CursorTimer;
	bool m_CursorIsVisible;

	float m_CursorHorizontal;
	float m_CursorVerticalMinimum;
	float m_CursorVerticalMaximum;

	float m_CellHeight;

	static std::vector<ConsoleLine, UntrackedAllocator<ConsoleLine>> s_AllConsoleLines;
	std::string m_CurrentInput;

	ProportionalFont* m_ConsoleFont;

	Vector2 m_NewestConsoleLineMinimums;
	Vector2 m_LiveInputMinimums;

	static std::map<const char*, ConsoleCommandCallBack*, CompareCStrings, UntrackedAllocator<std::pair<const char*, ConsoleCommandCallBack*>>> s_AllRegisteredCommands;
	static std::vector<std::string, UntrackedAllocator<std::string>> s_AllCommandDescriptions;
};



void ClearConsole(Command& currentCommand);
void ShowHelp(Command& currentCommand);
void QuitApplication(Command& currentCommand);

extern DeveloperConsole* g_DeveloperConsole;