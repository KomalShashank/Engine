#include "Engine/Networking/TCP-IP/RemoteCommandService.hpp"
#include "Engine/DeveloperConsole/DeveloperConsole.hpp"



RemoteCommandService* g_RemoteCommandService = nullptr;



RemoteServiceConnection::RemoteServiceConnection(TCPConnection* newConnection) :
m_Connection(newConnection)
{

}



void RemoteServiceConnection::SendMessageOnConnection(const unsigned char messageID, const char* messageData)
{
	messageID; // TODO: Define this function.
	messageData; // TODO: Define this function.
}



void RemoteServiceConnection::ReceiveMessageOnConnection()
{

}



bool RemoteServiceConnection::IsServiceConnected()
{
	if (m_Connection != nullptr)
	{
		if (m_Connection->IsTCPConnected())
		{
			return true;
		}
	}

	return false;
}



RemoteCommandService::RemoteCommandService() :
m_Listener(nullptr)
{
	m_OnJoiningConnection.RegisterMethod(this, &RemoteCommandService::AddTCPConnection);
	NetworkSystem::SingletonInstance()->m_OnUpdate.RegisterMethod(this, &RemoteCommandService::Update);

	DeveloperConsole::RegisterCommands("ListTCPAddresses", "List all the addresses available for a given port number. Takes port number as argument.", ListTCPAddressesCommand);
	DeveloperConsole::RegisterCommands("HostService", "Hosts a new TCP Service. Takes the host name and port number as arguments.", ServerHostCommand);
	DeveloperConsole::RegisterCommands("StopService", "Stops the currently hosted TCP Service.", ServerStopCommand);
	DeveloperConsole::RegisterCommands("JoinService", "Joins a hosted TCP Service. Takes the host name and port number as arguments.", ServerJoinCommand);
	DeveloperConsole::RegisterCommands("LeaveService", "Leaves the currently joined TCP Service.", ServerLeaveCommand);
	DeveloperConsole::RegisterCommands("ListServiceInfo", "Lists the current TCP Service information.", ServerInfoCommand);
	DeveloperConsole::RegisterCommands("SendCommand", "Unavailable as of now.", SendCommandCommand); // TODO: Refactor this.
}



void RemoteCommandService::CreateRemoteCommandService()
{
	if (g_RemoteCommandService == nullptr)
	{
		g_RemoteCommandService = new RemoteCommandService();
	}
}



bool RemoteCommandService::HostService(const char* hostName, const char* servicePort)
{
	m_Listener = new TCPListener(hostName, servicePort);

	if (m_Listener->IsListening())
	{
		return true;
	}

	return false;
}



bool RemoteCommandService::StopService()
{
	if (m_Listener != nullptr)
	{
		if (m_Listener->IsListening())
		{
			m_Listener->StopListener();
			if (!m_Listener->IsListening())
			{
				return true;
			}
		}
	}

	return false;
}



bool RemoteCommandService::JoinService(const char* hostName, const char* servicePort)
{
	TCPConnection* newConnection = new TCPConnection(hostName, servicePort);

	if (newConnection->IsTCPConnected())
	{
		RemoteServiceConnection* remoteServiceConnection = new RemoteServiceConnection(newConnection);
		m_OnJoiningConnection.TriggerEvent(remoteServiceConnection);

		return true;
	}

	delete newConnection;
	return false;
}



bool RemoteCommandService::LeaveService()
{
	bool leaveSuccessful = false;
	if(IsServiceConnected())
	{
		leaveSuccessful = true;
		for (RemoteServiceConnection* currentConnection : m_Connections)
		{
			if (currentConnection->IsServiceConnected())
			{
				currentConnection->m_Connection->DisconnectTCP();
				if (currentConnection->IsServiceConnected())
				{
					leaveSuccessful &= false;
				}
			}
		}
	}

	return leaveSuccessful;
}



void RemoteCommandService::Update()
{
	CheckForConnection();
	CheckForMessages();
	CheckForDisconnection();
}



bool RemoteCommandService::IsServiceListening()
{
	if (m_Listener != nullptr)
	{
		if (m_Listener->IsListening())
		{
			return true;
		}
	}

	return false;
}



bool RemoteCommandService::IsServiceConnected()
{
	if (!m_Connections.empty())
	{
		for (RemoteServiceConnection* currentConnection : m_Connections)
		{
			if (currentConnection->IsServiceConnected())
			{
				return true;
			}
		}
	}

	return false;
}



const char* RemoteCommandService::GetServerHostName()
{
	if (IsServiceListening())
	{
		return NetworkSystem::SingletonInstance()->GetHostNameForAddress((sockaddr*)(&m_Listener->m_Address));
	}

	return nullptr;
}



const char* RemoteCommandService::GetServerAddress()
{
	if (IsServiceListening())
	{
		return NetworkSystem::SingletonInstance()->GetSocketAddressAsString((sockaddr*)(&m_Listener->m_Address));
	}

	return nullptr;
}



const char* RemoteCommandService::GetClientHostName()
{
	RemoteServiceConnection* currentConnection = nullptr;
	if (!m_Connections.empty())
	{
		currentConnection = m_Connections[0];
		if (currentConnection != nullptr)
		{
			if (currentConnection->IsServiceConnected())
			{
				return NetworkSystem::SingletonInstance()->GetHostNameForAddress((sockaddr*)(&currentConnection->m_Connection->m_Address));
			}
		}
	}

	return nullptr;
}



const char* RemoteCommandService::GetClientAddress()
{
	RemoteServiceConnection* currentConnection = nullptr;
	if (!m_Connections.empty())
	{
		currentConnection = m_Connections[0];
		if (currentConnection != nullptr)
		{
			if (currentConnection->IsServiceConnected())
			{
				return NetworkSystem::SingletonInstance()->GetSocketAddressAsString((sockaddr*)(&currentConnection->m_Connection->m_Address));
			}
		}
	}

	return nullptr;
}



void RemoteCommandService::GetAllClientNamesAndAddresses(std::vector<std::string>& infoList)
{
	for (RemoteServiceConnection* currentConnection : m_Connections)
	{
		if (currentConnection->IsServiceConnected())
		{
			const char* currentName = NetworkSystem::SingletonInstance()->GetHostNameForAddress((sockaddr*)(&currentConnection->m_Connection->m_Address));
			const char* currentAddress = NetworkSystem::SingletonInstance()->GetSocketAddressAsString((sockaddr*)(&currentConnection->m_Connection->m_Address));
			infoList.push_back(std::string(currentName) + " " + std::string(currentAddress));
		}
	}
}



uint32_t RemoteCommandService::GetServiceState()
{
	if (IsServiceListening())
	{
		return SERVER;
	}

	for (RemoteServiceConnection* currentConnection : m_Connections)
	{
		if (currentConnection->IsServiceConnected())
		{
			return CLIENT;
		}
	}

	return NONE;
}



void RemoteCommandService::CheckForConnection()
{
	if (m_Listener != nullptr)
	{
		if (m_Listener->IsListening())
		{
			TCPConnection* newConnection = m_Listener->AcceptConnection();
			if (newConnection != nullptr)
			{
				RemoteServiceConnection* remoteServiceConnection = new RemoteServiceConnection(newConnection);
				m_OnJoiningConnection.TriggerEvent(remoteServiceConnection);
			}
		}
	}
}



void RemoteCommandService::CheckForMessages()
{

}



void RemoteCommandService::CheckForDisconnection()
{
	for (auto connectionIterator = m_Connections.begin(); connectionIterator != m_Connections.end();)
	{
		RemoteServiceConnection* currentConnection = *connectionIterator;
		if (currentConnection != nullptr)
		{
			if (!currentConnection->IsServiceConnected())
			{
				connectionIterator = m_Connections.erase(connectionIterator);
				delete currentConnection;
				
				continue;
			}
		}

		++connectionIterator;
	}
}



void RemoteCommandService::AddTCPConnection(RemoteServiceConnection* remoteServiceConnection)
{
	remoteServiceConnection->m_OnMessage.RegisterMethod(this, &RemoteCommandService::ConnectedMessage);
	m_Connections.push_back(remoteServiceConnection);
}



void RemoteCommandService::ConnectedMessage(RemoteServiceConnection* remoteServiceConnection, const unsigned char messageID, const char* messageData)
{
	m_OnMessage.TriggerEvent(remoteServiceConnection, messageID, messageData);
}



void ListTCPAddressesCommand(Command& currentCommand)
{
	ConsoleLine listMessage;

	if (currentCommand.HasNoArguments())
	{
		listMessage = ConsoleLine("Cannot list addresses. No port number given.", RGBA::RED);
	}
	else
	{
		std::vector<std::string> currentCommandArguments;
		currentCommand.GetCommandArguments(currentCommandArguments);
		const char* servicePort = currentCommandArguments[0].c_str();

		std::vector<std::string> addressList;
		NetworkSystem::SingletonInstance()->GetAllAddressesForPort(servicePort, addressList);

		for (const std::string& currentAddress : addressList)
		{
			ConsoleLine addressLine = ConsoleLine(currentAddress.c_str(), RGBA::YELLOW);
			DeveloperConsole::AddNewConsoleLine(addressLine);
		}

		listMessage = ConsoleLine("All available addresses listed.", RGBA::GREEN);
	}

	DeveloperConsole::AddNewConsoleLine(listMessage);
}



void ServerHostCommand(Command& currentCommand)
{
	ConsoleLine hostMessage;

	if (currentCommand.HasNoArguments())
	{
		if (g_RemoteCommandService->IsServiceListening())
		{
			hostMessage = ConsoleLine("Cannot host service. Service is already connected.", RGBA::RED);
		}
		else
		{
			bool hostSuccessful = g_RemoteCommandService->HostService(NetworkSystem::SingletonInstance()->GetLocalHostName(), "4325");
			if (hostSuccessful)
			{
				hostMessage = ConsoleLine(Stringf("Hosting service from connection: %s", g_RemoteCommandService->GetServerAddress()), RGBA::GREEN);
			}
			else
			{
				hostMessage = ConsoleLine("Hosting failed. No addresses found.", RGBA::RED);
			}
		}
	}
	else
	{
		if (g_RemoteCommandService->IsServiceListening())
		{
			hostMessage = ConsoleLine("Cannot host service. Service is already connected.", RGBA::RED);
		}
		else
		{
			std::vector<std::string> currentCommandArguments;
			currentCommand.GetCommandArguments(currentCommandArguments);
			if (currentCommandArguments.size() == 2)
			{
				const char* hostName = currentCommandArguments[0].c_str();
				const char* servicePort = currentCommandArguments[1].c_str();

				bool hostSuccessful = g_RemoteCommandService->HostService(hostName, servicePort);
				if (hostSuccessful)
				{
					hostMessage = ConsoleLine(Stringf("Hosting service from connection: %s", g_RemoteCommandService->GetServerAddress()), RGBA::GREEN);
				}
				else
				{
					hostMessage = ConsoleLine("Hosting failed. No addresses found.", RGBA::RED);
				}
			}
			else
			{
				hostMessage = ConsoleLine("Cannot host service. Commands needs two arguments.", RGBA::RED);
			}
		}
	}

	DeveloperConsole::AddNewConsoleLine(hostMessage);
}



void ServerStopCommand(Command& currentCommand)
{
	ConsoleLine stopMessage;

	if (currentCommand.HasNoArguments())
	{
		if (g_RemoteCommandService->IsServiceListening())
		{
			bool stopSuccessful = g_RemoteCommandService->StopService();
			if (stopSuccessful)
			{
				stopMessage = ConsoleLine("Service stopped.", RGBA::GREEN);
			}
			else
			{
				stopMessage = ConsoleLine("Unable to stop service.", RGBA::RED);
			}
		}
		else
		{
			stopMessage = ConsoleLine("No service connected currently.", RGBA::RED);
		}
	}
	else
	{
		stopMessage = ConsoleLine("Cannot stop service. Stop command does not take any arguments.", RGBA::RED);
	}

	DeveloperConsole::AddNewConsoleLine(stopMessage);
}



void ServerJoinCommand(Command& currentCommand)
{
	ConsoleLine joinMessage;

	if (currentCommand.HasNoArguments())
	{
		joinMessage = ConsoleLine("Cannot join service. No service name given.", RGBA::RED);
	}
	else
	{
		if (g_RemoteCommandService->IsServiceConnected())
		{
			joinMessage = ConsoleLine("Cannot join service. Service is already connected.", RGBA::RED);
		}
		else
		{
			std::vector<std::string> currentCommandArguments;
			currentCommand.GetCommandArguments(currentCommandArguments);
			if (currentCommandArguments.size() == 2)
			{
				const char* hostName = currentCommandArguments[0].c_str();
				const char* servicePort = currentCommandArguments[1].c_str();

				bool joinSuccessful = g_RemoteCommandService->JoinService(hostName, servicePort);
				if (joinSuccessful)
				{
					joinMessage = ConsoleLine(Stringf("Joining service from connection: %s", g_RemoteCommandService->GetClientAddress()), RGBA::GREEN);
				}
				else
				{
					joinMessage = ConsoleLine("Joining failed. No host found.", RGBA::RED);
				}
			}
			else
			{
				joinMessage = ConsoleLine("Cannot join service. Commands needs two arguments.", RGBA::RED);
			}
		}
	}

	DeveloperConsole::AddNewConsoleLine(joinMessage);
}



void ServerLeaveCommand(Command& currentCommand)
{
	ConsoleLine leaveMessage;

	if (currentCommand.HasNoArguments())
	{
		if (g_RemoteCommandService->IsServiceConnected())
		{
			bool leaveSuccessful = g_RemoteCommandService->LeaveService();
			if (leaveSuccessful)
			{
				leaveMessage = ConsoleLine("Left service.", RGBA::GREEN);
			}
			else
			{
				leaveMessage = ConsoleLine("Unable to leave service.", RGBA::RED);
			}
		}
		else
		{
			leaveMessage = ConsoleLine("No service connected currently.", RGBA::RED);
		}
	}
	else
	{
		leaveMessage = ConsoleLine("Cannot leave service. Leave command does not take any arguments.", RGBA::RED);
	}

	DeveloperConsole::AddNewConsoleLine(leaveMessage);
}



void ServerInfoCommand(Command& currentCommand)
{
	ConsoleLine infoMessage;

	if (currentCommand.HasNoArguments())
	{
		uint32_t serviceState = g_RemoteCommandService->GetServiceState();
		switch (serviceState)
		{
		case NONE:
		{
			ConsoleLine serviceStateLine = ConsoleLine("NONE", RGBA::YELLOW);
			DeveloperConsole::AddNewConsoleLine(serviceStateLine);
		}
		break;

		case SERVER:
		{
			ConsoleLine serviceStateLine = ConsoleLine(Stringf("SERVER: %s", g_RemoteCommandService->GetServerAddress()), RGBA::YELLOW);
			DeveloperConsole::AddNewConsoleLine(serviceStateLine);

			std::vector<std::string> clientInfoList;
			g_RemoteCommandService->GetAllClientNamesAndAddresses(clientInfoList);

			for (const std::string& currentinfo : clientInfoList)
			{
				ConsoleLine addressLine = ConsoleLine(currentinfo, RGBA::YELLOW);
				DeveloperConsole::AddNewConsoleLine(addressLine);
			}
		}
		break;

		case CLIENT:
		{
			ConsoleLine serviceStateLine = ConsoleLine(Stringf("CLIENT: %s", g_RemoteCommandService->GetClientAddress()), RGBA::YELLOW);
			DeveloperConsole::AddNewConsoleLine(serviceStateLine);
		}
		break;

		default:
			break;
		}

		infoMessage = ConsoleLine("Listing service info.", RGBA::GREEN);
	}
	else
	{
		infoMessage = ConsoleLine("Cannot list info. Info command does not take any arguments.", RGBA::RED);
	}

	DeveloperConsole::AddNewConsoleLine(infoMessage);
}



void SendCommandCommand(Command& currentCommand) // TODO: Finish this command.
{
	ConsoleLine sendMessage;

	if (currentCommand.HasNoArguments())
	{
		sendMessage = ConsoleLine("Cannot send command. No command given.", RGBA::RED);
	}
	else
	{

	}

	DeveloperConsole::AddNewConsoleLine(sendMessage);
}