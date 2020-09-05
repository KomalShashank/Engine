#include "Engine/DeveloperConsole/DeveloperConsole.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderUtilities/BasicRenderer.hpp"



DeveloperConsole* g_DeveloperConsole = nullptr;

const float OFFSET = 0.1f;



std::vector<ConsoleLine, UntrackedAllocator<ConsoleLine>> DeveloperConsole::s_AllConsoleLines;

std::map<const char*, ConsoleCommandCallBack*, CompareCStrings, UntrackedAllocator<std::pair<const char*, ConsoleCommandCallBack*>>> DeveloperConsole::s_AllRegisteredCommands;
std::vector<std::string, UntrackedAllocator<std::string>> DeveloperConsole::s_AllCommandDescriptions;



ConsoleLine::ConsoleLine() :
m_ConsoleLineText(""),
m_ConsoleLineColor(RGBA::WHITE)
{

}



ConsoleLine::ConsoleLine(const std::string& consoleLineText, const RGBA& consoleLineColor /*= RGBA::WHITE*/) :
m_ConsoleLineText(consoleLineText),
m_ConsoleLineColor(consoleLineColor)
{

}



DeveloperConsole::DeveloperConsole(const Vector2& viewDimensions) :
m_ConsoleIsOpen(false),
m_CursorTimer(0.0f),
m_CursorIsVisible(false),
m_ConsoleFont(nullptr)
{
	m_ScreenBounds = AABB2(Vector2::ZERO, viewDimensions);

	m_PromptBoxBounds = AABB2(Vector2(OFFSET, OFFSET), Vector2(viewDimensions.X - OFFSET, OFFSET + 0.5f));
	m_SurroundingBoxBounds = AABB2(Vector2::ZERO, Vector2(viewDimensions.X, (OFFSET * 2.0f) + 0.5f));

	m_NewestConsoleLineMinimums = Vector2(m_PromptBoxBounds.minimums.X, m_PromptBoxBounds.maximums.Y + (OFFSET * 2.0f));

	m_CursorHorizontal = m_PromptBoxBounds.minimums.X + OFFSET;

	m_CursorVerticalMinimum = m_PromptBoxBounds.minimums.Y + OFFSET;
	m_CursorVerticalMaximum = m_PromptBoxBounds.maximums.Y - OFFSET;

	m_CellHeight = m_CursorVerticalMaximum - m_CursorVerticalMinimum;

	m_LiveInputMinimums = Vector2(m_CursorHorizontal, m_CursorVerticalMinimum + 0.025f);

	RegisterCommands("Clear", "Clears the console.", ClearConsole);
	RegisterCommands("Help", "Shows helpful information.", ShowHelp);
	RegisterCommands("Quit", "Quit the application.", QuitApplication);

	m_ConsoleFont = ProportionalFont::CreateOrGetProportionalFont("Arial");

	ConsoleLine welcomeLine = ConsoleLine("Welcome to the Command Console. Type Help for a list of available commands.");
	DeveloperConsole::AddNewConsoleLine(welcomeLine);
}



void DeveloperConsole::Update(float deltaTimeInSeconds)
{
	ToggleConsole();
	UpdateCursor(deltaTimeInSeconds);
	ProcessInputForConsole();
	ScrollThroughConsole();
}



void DeveloperConsole::Render() const
{
	if (m_ConsoleIsOpen)
	{
		g_BasicRenderer->SetOrthographicProjection(m_ScreenBounds.minimums, m_ScreenBounds.maximums);
		g_BasicRenderer->EnableDepthTesting(false);

		g_BasicRenderer->DrawSingle2DQuadVA(m_ScreenBounds, false, RGBA(0, 0, 0, 128));
		PrintAllConsoleLines();

		g_BasicRenderer->DrawSingle2DQuadVA(m_SurroundingBoxBounds, false, RGBA::BLACK);
		g_BasicRenderer->DrawSingle2DQuadVA(m_PromptBoxBounds, true);
		PrintCurrentLiveInput();

		float horizontalCursorDisplacement = m_CursorHorizontal + CalculateTextWidth(m_CurrentInput, m_CellHeight, m_ConsoleFont);

		if (m_CursorIsVisible)
		{
			Vector2 cursorStartingPoint = Vector2(horizontalCursorDisplacement, m_CursorVerticalMinimum);
			Vector2 cursorEndingPoint = Vector2(horizontalCursorDisplacement, m_CursorVerticalMaximum);

			g_BasicRenderer->DrawSingle2DLineVA(cursorStartingPoint, cursorEndingPoint, 1.5f);
		}

		g_BasicRenderer->EnableDepthTesting(true);
	}
}



float DeveloperConsole::CalculateTextWidth(const std::string& asciiText, float cellHeight, const ProportionalFont* font /*= nullptr*/) const
{
	float textWidth = 0.0f;
	float textScale = font->GetCharacterScaleForCellHeight(cellHeight);

	ProportionalGlyph* previousGlyph = nullptr;

	for (char asciiGlyph : asciiText)
	{
		ProportionalGlyph* currentGlyph = font->GetProportionalGlyphForGlyphUnicode(asciiGlyph);
		if (currentGlyph == nullptr)
		{
			continue;
		}

		float currentGlyphKerning = 0.0f;
		if (previousGlyph != nullptr && currentGlyph != nullptr)
		{
			currentGlyphKerning = font->GetKerningForGlyphPair(previousGlyph->m_glyphUnicode, currentGlyph->m_glyphUnicode);
		}

		textWidth += currentGlyphKerning * textScale;

		textWidth += currentGlyph->GetAdvance().X * textScale;
		previousGlyph = currentGlyph;
	}

	return textWidth;
}



bool DeveloperConsole::IsOpen() const
{
	return m_ConsoleIsOpen;
}



void DeveloperConsole::ToggleConsole()
{
	if (InputSystem::SingletonInstance()->KeyWasJustPressed(192))
	{
		m_ConsoleIsOpen = !m_ConsoleIsOpen;
	}
}



void DeveloperConsole::CloseConsole()
{
	m_ConsoleIsOpen = false;
}



void DeveloperConsole::UpdateCursor(float deltaTimeInSeconds)
{
	if (m_ConsoleIsOpen)
	{
		m_CursorTimer += deltaTimeInSeconds;
		if (m_CursorTimer > 0.25f)
		{
			m_CursorIsVisible = !m_CursorIsVisible;
			m_CursorTimer = 0.0f;
		}
	}
	else
	{
		m_CursorTimer = 0.0f;
	}
}



void DeveloperConsole::ProcessInputForConsole()
{
	if (m_ConsoleIsOpen)
	{
		if (m_CurrentInput.empty())
		{
			if (InputSystem::SingletonInstance()->KeyWasJustPressed(ENTER_KEY) || InputSystem::SingletonInstance()->KeyWasJustPressed(ESCAPE_KEY))
			{
				m_ConsoleIsOpen = false;
			}
		}
		else
		{
			if (InputSystem::SingletonInstance()->KeyWasJustPressed(BACKSPACE_KEY))
			{
				m_CurrentInput.pop_back();
			}
			else if (InputSystem::SingletonInstance()->KeyWasJustPressed(ENTER_KEY))
			{
				ConsoleLine newConsoleLine = ConsoleLine(m_CurrentInput);
				ResetNewestConsoleLineMinimums();
				AddNewConsoleLine(newConsoleLine);
				RunConsoleCommand(m_CurrentInput.c_str());

				m_CurrentInput.clear();
			}
			else if (InputSystem::SingletonInstance()->KeyWasJustPressed(ESCAPE_KEY))
			{
				m_CurrentInput.clear();
			}
		}
	}
	else
	{
		m_CurrentInput.clear();
	}
}



void DeveloperConsole::ScrollThroughConsole()
{
	if (m_ConsoleIsOpen)
	{
		if (InputSystem::SingletonInstance()->GetMouseWheelDirection() < 0)
		{
			m_NewestConsoleLineMinimums.Y += m_ConsoleFont->GetLineSpacingForCellHeight(m_CellHeight) * 2.0f;

			if (m_NewestConsoleLineMinimums.Y > m_PromptBoxBounds.maximums.Y + (OFFSET * 2.0f))
			{
				m_NewestConsoleLineMinimums.Y = m_PromptBoxBounds.maximums.Y + (OFFSET * 2.0f);
			}
		}
		else if (InputSystem::SingletonInstance()->GetMouseWheelDirection() > 0)
		{
			m_NewestConsoleLineMinimums.Y -= m_ConsoleFont->GetLineSpacingForCellHeight(m_CellHeight) * 2.0f;
		}
	}
	else
	{
		ResetNewestConsoleLineMinimums();
	}
}



void DeveloperConsole::ResetNewestConsoleLineMinimums()
{
	m_NewestConsoleLineMinimums = Vector2(m_PromptBoxBounds.minimums.X, m_PromptBoxBounds.maximums.Y + (OFFSET * 2.0f));
}



void DeveloperConsole::GetLiveInput(unsigned char inputCharacter)
{
	if (m_ConsoleIsOpen)
	{
		if (inputCharacter != '`' && inputCharacter != '~' && inputCharacter != 8 && inputCharacter != 13 && inputCharacter != 27)
		{
			m_CurrentInput.push_back(inputCharacter);
		}
	}
}



void DeveloperConsole::PrintCurrentLiveInput() const
{
	g_BasicRenderer->Draw2DProportionalText(m_LiveInputMinimums, m_CurrentInput, m_CellHeight, m_ConsoleFont);
}



void DeveloperConsole::PrintLineToConsole(const Vector2& startingLetterMinimums, float cellHeight, const ConsoleLine& consoleLine) const
{
	g_BasicRenderer->Draw2DProportionalText(startingLetterMinimums, consoleLine.m_ConsoleLineText, cellHeight, m_ConsoleFont, consoleLine.m_ConsoleLineColor);
}



void DeveloperConsole::PrintAllConsoleLines() const
{
	Vector2 startingLetterMinimums = m_NewestConsoleLineMinimums;
	float lineSpacing = m_ConsoleFont->GetLineSpacingForCellHeight(m_CellHeight);

	for (auto consoleLineReverseIterator = s_AllConsoleLines.rbegin(); consoleLineReverseIterator != s_AllConsoleLines.rend(); ++consoleLineReverseIterator)
	{
		ConsoleLine consoleLine = *consoleLineReverseIterator;
		PrintLineToConsole(startingLetterMinimums, m_CellHeight, consoleLine);

		startingLetterMinimums.Y += lineSpacing;
	}
}



void DeveloperConsole::AddNewConsoleLine(const ConsoleLine& newConsoleLine)
{
	s_AllConsoleLines.push_back(newConsoleLine);
}



void DeveloperConsole::ClearAllConsoleLines()
{
	s_AllConsoleLines.clear();
}



bool DeveloperConsole::ConsoleIsEmpty()
{
	return s_AllConsoleLines.empty();
}



void DeveloperConsole::RegisterCommands(const char* commandName, const char* commandDescription, ConsoleCommandCallBack* commandCallBack)
{
	auto registeredCommandsIterator = s_AllRegisteredCommands.find(commandName);
	if (registeredCommandsIterator == s_AllRegisteredCommands.end())
	{
		s_AllRegisteredCommands[commandName] = commandCallBack;
		s_AllCommandDescriptions.push_back(Stringf("%s - %s", commandName, commandDescription));
	}
}



void DeveloperConsole::RunConsoleCommand(const char* consoleCommand)
{
	Command currentCommand = Command(consoleCommand);

	std::string currentCommandName;
	currentCommand.GetCommandName(currentCommandName);

	auto registeredCommandsIterator = s_AllRegisteredCommands.find(currentCommandName.c_str());
	if (registeredCommandsIterator == s_AllRegisteredCommands.end())
	{
		ConsoleLine invalidCommand = ConsoleLine("Invalid command. Type help for help options.", RGBA::RED);
		AddNewConsoleLine(invalidCommand);
	}
	else
	{
		ConsoleCommandCallBack* currentCommandCallBack = registeredCommandsIterator->second;
		currentCommandCallBack(currentCommand);
	}
}



std::vector<std::string, UntrackedAllocator<std::string>>& DeveloperConsole::GetAllCommandDescriptions()
{
	return s_AllCommandDescriptions;
}



void ClearConsole(Command& currentCommand)
{
	if(currentCommand.HasNoArguments())
	{
		if (!g_DeveloperConsole->ConsoleIsEmpty())
		{
			DeveloperConsole::ClearAllConsoleLines();
		}

		ConsoleLine clearedMessage = ConsoleLine("Console cleared.", RGBA::BLUE);
		DeveloperConsole::AddNewConsoleLine(clearedMessage);
	}
	else
	{
		ConsoleLine failedMessage = ConsoleLine("Clear command takes no arguments.", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(failedMessage);
	}
}



void ShowHelp(Command& currentCommand)
{
	if (currentCommand.HasNoArguments())
	{
		ConsoleLine helpMessage;

		helpMessage = ConsoleLine("Console Help:", RGBA::GREEN);
		DeveloperConsole::AddNewConsoleLine(helpMessage);
		
		helpMessage = ConsoleLine("Press ~ to open and dismiss the console.");
		DeveloperConsole::AddNewConsoleLine(helpMessage);
		
		helpMessage = ConsoleLine("Hit Enter or Escape on empty input to close the console.");
		DeveloperConsole::AddNewConsoleLine(helpMessage);
		
		helpMessage = ConsoleLine("Hit Enter on non-empty input to run the console command.");
		DeveloperConsole::AddNewConsoleLine(helpMessage);
		
		helpMessage = ConsoleLine("Hit Escape on non-empty input to clear the input.");
		DeveloperConsole::AddNewConsoleLine(helpMessage);

		helpMessage = ConsoleLine("List of Commands (All commands are case-sensitive):", RGBA::GREEN);
		DeveloperConsole::AddNewConsoleLine(helpMessage);

		for (auto currentCommandDescription : DeveloperConsole::GetAllCommandDescriptions())
		{
			helpMessage = ConsoleLine(currentCommandDescription);
			DeveloperConsole::AddNewConsoleLine(helpMessage);
		}
	}
	else
	{
		ConsoleLine failedMessage = ConsoleLine("Help command takes no arguments.", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(failedMessage);
	}
}



void QuitApplication(Command& currentCommand)
{
	if (currentCommand.HasNoArguments())
	{
		ConsoleLine quitMessage = ConsoleLine("To quit, close the console and then hit Escape.", RGBA::YELLOW);
		DeveloperConsole::AddNewConsoleLine(quitMessage);
	}
	else
	{
		ConsoleLine failedMessage = ConsoleLine("Quit command takes no arguments.", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(failedMessage);
	}
}