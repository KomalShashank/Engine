#pragma once

#include <string>
#include <vector>



class Command
{
public:
	Command();
	Command(const std::string& newCommand);
	Command(const std::vector<std::string>& newCommand);

	void GetCommandName(std::string& commandName);
	void GetCommandArguments(std::vector<std::string>& commandArguments);

	bool HasNoArguments() const;

private:
	std::string m_CommandName;
	std::vector<std::string> m_CommandArguments;
};