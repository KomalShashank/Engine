#include "Engine/DeveloperConsole/Command.hpp"

#include <sstream>



Command::Command()
{

}



Command::Command(const std::string& newCommand)
{
	std::stringstream newCommandStream(newCommand);
	std::vector<std::string> newCommandSeparated;

	std::string word;
	while (newCommandStream >> word)
	{
		newCommandSeparated.push_back(word);
	}

	m_CommandName = newCommandSeparated.front();

	for (auto newCommandIterator = newCommandSeparated.begin() + 1; newCommandIterator != newCommandSeparated.end(); ++newCommandIterator)
	{
		std::string currentArgument = *newCommandIterator;
		m_CommandArguments.push_back(currentArgument);
	}
}



Command::Command(const std::vector<std::string>& newCommand)
{
	m_CommandName = newCommand.front();

	for (auto newCommandIterator = newCommand.begin() + 1; newCommandIterator != newCommand.end(); ++newCommandIterator)
	{
		std::string currentArgument = *newCommandIterator;
		m_CommandArguments.push_back(currentArgument);
	}
}



void Command::GetCommandName(std::string& commandName)
{
	commandName = m_CommandName;
}



void Command::GetCommandArguments(std::vector<std::string>& commandArguments)
{
	commandArguments = m_CommandArguments;
}



bool Command::HasNoArguments() const
{
	return m_CommandArguments.empty();
}