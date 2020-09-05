#pragma once

#include <vector>

#include "Engine/EventSystem/EventSystem.hpp"
#include "Engine/DeveloperConsole/Command.hpp"
#include "Engine/Networking/TCP-IP/TCPListener.hpp"
#include "Engine/Networking/TCP-IP/TCPConnection.hpp"



#define NONE 0
#define SERVER 1
#define CLIENT 2



class RemoteServiceConnection
{
public:
	RemoteServiceConnection(TCPConnection* newConnection);

	void SendMessageOnConnection(const unsigned char messageID, const char* messageData);
	void ReceiveMessageOnConnection();

	bool IsServiceConnected();

public:
	TCPConnection* m_Connection;
	std::vector<char> m_NextMessage;

	EventSystem<RemoteServiceConnection*, unsigned char, const char*> m_OnMessage;
};



class RemoteCommandService
{
private:
	RemoteCommandService();

public:
	static void CreateRemoteCommandService();
	bool HostService(const char* hostName, const char* servicePort);
	bool StopService();
	bool JoinService(const char* hostName, const char* servicePort);
	bool LeaveService();

	void Update();

	bool IsServiceListening();
	bool IsServiceConnected();

	const char* GetServerHostName();
	const char* GetServerAddress();

	const char* GetClientHostName();
	const char* GetClientAddress();
	
	void GetAllClientNamesAndAddresses(std::vector<std::string>& infoList);

	uint32_t GetServiceState();

private:
	void CheckForConnection();
	void CheckForMessages();
	void CheckForDisconnection();

	void AddTCPConnection(RemoteServiceConnection* remoteServiceConnection);
	void ConnectedMessage(RemoteServiceConnection* remoteServiceConnection, const unsigned char messageID, const char* messageData);

public:
	TCPListener* m_Listener;
	std::vector<RemoteServiceConnection*> m_Connections;

	EventSystem<RemoteServiceConnection*> m_OnJoiningConnection;
	EventSystem<RemoteServiceConnection*> m_OnLeavingConnection;
	EventSystem<RemoteServiceConnection*, const unsigned char, const char*> m_OnMessage;
};

void ListTCPAddressesCommand(Command& currentCommand);
void ServerHostCommand(Command& currentCommand);
void ServerStopCommand(Command& currentCommand);
void ServerJoinCommand(Command& currentCommand);
void ServerLeaveCommand(Command& currentCommand);
void ServerInfoCommand(Command& currentCommand);
void SendCommandCommand(Command& currentCommand);