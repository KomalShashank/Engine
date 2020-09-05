#pragma once

#include <vector>

#include "Engine/Networking/UDP/NetConnection.hpp"
#include "Engine/Networking/UDP/PacketChannel.hpp"
#include "Engine/DeveloperConsole/Command.hpp"
#include "Engine/EventSystem/EventSystem.hpp"



const size_t NUMBER_OF_MESSAGE_DEFINITIONS = 256;
const uint16_t STARTING_PORT_NUMBER = 1608;
const uint16_t PORT_RANGE = 8;
const uint8_t HOST_CONNECTION_INDEX = 0;
const size_t MAXIMUM_NUMBER_OF_CONNECTIONS = 8;

const float UPDATE_RATE = 1.0f / 120.0f;
const float TIMEOUT_THRESHOLD = 15.0f;



enum NetSessionState
{
	INITIAL_STATE,
	UNCONNECTED_STATE,
	CONNECTED_STATE,
	HOSTING_STATE,
	JOINING_STATE
};



enum ErrorCode : uint8_t
{
	NO_ERROR_CODE = 0,
	START_ERROR_FAILED_TO_CREATE_SOCKET,
	JOIN_ERROR_HOST_TIMED_OUT,
	JOIN_DENIED_HOST_NOT_LISTENING,
	JOIN_DENIED_NOT_HOST,
	JOIN_DENIED_FULL,
	JOIN_DENIED_GUID_IN_USE,
	ERROR_HOST_DISCONNECTED
};



class NetSession
{
private:
	NetSession();
	~NetSession();

public:
	static void CreateNetSession();
	static void DestroyNetSession();

	static NetSession* LocalNetSession();

	void StartSessionOnPort(const char* servicePort);
	
	void Update();
	void RenderDebugDisplay(const Vector2& viewDimensions, const class ProportionalFont* displayFont);

	bool HostSession(const char* globalUniqueID);
	void JoinSession(const char* globalUniqueID, const sockaddr_in& hostAddress);
	void LeaveSession();

	bool ConnectConnection(NetConnection* newConnection, uint8_t connectionIndex);
	void DisconnectConnection(NetConnection* correspondingConnection);
	void FlushToAllConnections();

	NetConnection* CreateConnection(uint8_t connectionIndex, const char* globalUniqueID, const sockaddr_in& connectionAddress);
	void DeleteConnection(uint8_t connectionIndex);

	void SendDirectMessage(const sockaddr_in& toAddress, NetMessage& currentMessage);
	void RegisterMessage(uint8_t messageID, uint8_t controlFlags, uint8_t optionFlags, NetMessageCallBack* messageCallBack);

	void SendToHostConnection(NetMessage& currentMessage);
	void SendToAllConnections(NetMessage& currentMessage);

	const sockaddr_in GetSessionAddress() const;
	const char* GetSessionAddressAsString() const;
	bool IsSessionRunning() const;

	void SetListening(bool listening);
	bool IsHostListening() const;

	NetConnection* GetHostConnection() const;
	NetConnection* GetLocalConnection() const;

	uint8_t GetHostConnectionIndex() const;
	uint8_t GetLocalConnectionIndex() const;

	bool IsOwnConnectionHost() const;
	bool IsGlobalUniqueIDAvailable(const char* globalUniqueID) const;
	bool IsSessionFull() const;

	void SetCurrentState(const NetSessionState& currentState);
	NetSessionState GetCurrentState() const;
	bool IsCurrentState(const NetSessionState& currentState) const;

	NetConnection* GetConnectionAtIndex(uint8_t connectionIndex) const;
	NetConnection* GetConnectionWithName(const char* globalUniqueID) const;
	NetConnection* GetConnectionFromAddress(const sockaddr_in& connectionAddress) const;

	NetMessageDefinition* FindMessageDefinition(uint8_t messageID);
	void AddMessageDefinition(uint8_t messageID, const NetMessageDefinition& messageDefinition);

	void WriteConnectionInfo(NetMessage& currentMessage, const NetConnectionInfo& currentConnectionInfo);
	void ReadConnectionInfo(const NetMessage& currentMessage, NetConnectionInfo& currentConnectionInfo);

	void UpdateAndPrintLatestError(uint8_t latestError);
	void SendDenyMessage(uint8_t errorCode, const sockaddr_in& toAddress);

	NetConnection* AddNewConnection(const char* globalUniqueID, const sockaddr_in& connectionAddress);

private:
	void ProcessIncomingPackets();
	void UpdateAllSessionStates();

	void UpdateAllConnections();
	void TimeoutHostConnection();

	bool ReadNextPacketFromSocket(NetPacket& nextPacket, sockaddr_in& fromAddress);
	void ProcessPacketHeader(PacketHeader& packetHeader, NetPacket& receivedPacket, NetSender& fromSender);
	void ProcessNextMessageFromPacket(NetPacket& receivedPacket, NetMessage& currentMessage, NetSender& fromSender);

	bool MessageCanBeProcessed(const NetSender& fromSender, const NetMessage& currentMessage);

	bool IsOwnConnectionConnected() const;
	void FinalizeDisconnection(NetConnection*& currentConnection);

private:
	NetConnection* m_HostConnection;
	NetConnection* m_LocalConnection;

public:
	PacketChannel* m_PacketChannel;
	NetMessageDefinition m_AllMessageDefinitions[NUMBER_OF_MESSAGE_DEFINITIONS];

	std::vector<NetConnection*> m_AllConnections;

	NetSessionState m_CurrentState;
	
	float m_ElapsedTime;
	float m_HostTimeOut;

	float m_ElapsedTimeSinceLastSent;
	float m_ElapsedTimeSinceLastReceived;

	uint8_t m_LatestError;
	bool m_HostListening;

	EventSystem<NetConnection*> m_OnConnectionJoined;
	EventSystem<NetConnection*> m_OnConnectionLeft;
	EventSystem<NetConnection*> m_OnConnectionUpdated;
};

void OnPingReceived(const NetSender& pingSender, const NetMessage& pingMessage);
void OnPongReceived(const NetSender& pongSender, const NetMessage& pongMessage);

void OnJoinRequested(const NetSender& requestSender, const NetMessage& requestMessage);
void OnJoinDenied(const NetSender& denySender, const NetMessage& denyMessage);
void OnJoinAccepted(const NetSender& acceptSender, const NetMessage& acceptMessage);
void OnSessionLeft(const NetSender& leaveSender, const NetMessage& leaveMessage);

void StartSessionCommand(Command& currentCommand);
void StopSessionCommand(Command& currentCommand);
void PingMessageCommand(Command& currentCommand);

void HostSessionCommand(Command& currentCommand);
void JoinSessionCommand(Command& currentCommand);
void LeaveSessionCommand(Command& currentCommand);

void ListConnectionsCommand(Command& currentCommand);

void SimulateLagCommand(Command& currentCommand);
void SimulateLossCommand(Command& currentCommand);