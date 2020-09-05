#include "Engine/Networking/UDP/NetSession.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include "Engine/DeveloperConsole/DeveloperConsole.hpp"
#include "Engine/Renderer/RenderUtilities/BasicRenderer.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/Time/Clock.hpp"



NetSession* g_NetSession = nullptr;



NetSession::NetSession() :
m_PacketChannel(nullptr),
m_HostConnection(nullptr),
m_LocalConnection(nullptr),
m_CurrentState(INITIAL_STATE),
m_ElapsedTime(0.0f),
m_HostTimeOut(0.0f),
m_ElapsedTimeSinceLastSent(0.0f),
m_ElapsedTimeSinceLastReceived(0.0f),
m_LatestError(NO_ERROR_CODE),
m_HostListening(false)
{
	NetworkSystem::SingletonInstance()->m_OnUpdate.RegisterMethod(this, &NetSession::Update);

	DeveloperConsole::RegisterCommands("StartNetSession", "Starts a new Net Session.", StartSessionCommand);
	DeveloperConsole::RegisterCommands("StopNetSession", "Stops the currently running Net Session.", StopSessionCommand);
	DeveloperConsole::RegisterCommands("PingAddress", "Sends a ping to an address.", PingMessageCommand);

	DeveloperConsole::RegisterCommands("HostNetSession", "Creates a new Net Connection from the Net Session and hosts the Net Session.", HostSessionCommand);
	DeveloperConsole::RegisterCommands("JoinNetSession", "Creates a new Net Connection from the Net Session and joins an existing Host Net Session.", JoinSessionCommand);
	DeveloperConsole::RegisterCommands("LeaveNetSession", "Leaves the currently joined or hosted Net Session.", LeaveSessionCommand);

	DeveloperConsole::RegisterCommands("ListNetConnections", "Lists all the Net Connection connected in the current Net Session.", ListConnectionsCommand);

	DeveloperConsole::RegisterCommands("SimulateNetLag", "Arbitrarily sets a lag duration for the packets.", SimulateLagCommand);
	DeveloperConsole::RegisterCommands("SimulateNetLoss", "Arbitrarily sets a loss percentage for the packets.", SimulateLossCommand);

	RegisterMessage(NET_MESSAGE_PING, NMC_CONNECTIONLESS, NMO_UNRELIABLE, OnPingReceived);
	RegisterMessage(NET_MESSAGE_PONG, NMC_CONNECTIONLESS, NMO_UNRELIABLE, OnPongReceived);

	RegisterMessage(NET_MESSAGE_JOIN_REQUEST, NMC_CONNECTIONLESS, NMO_RELIABLE, OnJoinRequested);
	RegisterMessage(NET_MESSAGE_JOIN_DENY, NMC_CONNECTIONLESS, NMO_UNRELIABLE, OnJoinDenied);
	RegisterMessage(NET_MESSAGE_JOIN_ACCEPT, NMC_CONNECTED, NMO_RELIABLE | NMO_SEQUENCED, OnJoinAccepted);
	RegisterMessage(NET_MESSAGE_LEAVE, NMC_CONNECTED, NMO_UNRELIABLE, OnSessionLeft);
}



NetSession::~NetSession()
{
	if (IsSessionRunning())
	{
		PacketChannel::DestroyPacketChannel(m_PacketChannel);
	}

	NetworkSystem::SingletonInstance()->m_OnUpdate.UnregisterMethod(this, &NetSession::Update);
}



void NetSession::CreateNetSession()
{
	if (g_NetSession == nullptr)
	{
		g_NetSession = new NetSession();
	}
}



void NetSession::DestroyNetSession()
{
	if (g_NetSession != nullptr)
	{
		delete g_NetSession;
		g_NetSession = nullptr;
	}
}



NetSession* NetSession::LocalNetSession()
{
	ASSERT_OR_DIE(g_NetSession != nullptr, "A valid NetSession instance does not exist.");

	return g_NetSession;
}



void NetSession::StartSessionOnPort(const char* servicePort)
{
	m_PacketChannel = PacketChannel::CreatePacketChannel(NetworkSystem::SingletonInstance()->GetLocalHostName(), servicePort);

	if (!IsSessionRunning())
	{
		PacketChannel::DestroyPacketChannel(m_PacketChannel);
	}
}



void NetSession::Update()
{
	if (IsSessionRunning())
	{
		m_ElapsedTimeSinceLastSent += Clock::MasterClock()->GetDeltaTimeFloat();
		m_ElapsedTimeSinceLastReceived += Clock::MasterClock()->GetDeltaTimeFloat();
		
		ProcessIncomingPackets();
	}

	UpdateAllSessionStates();
}



void NetSession::RenderDebugDisplay(const Vector2& viewDimensions, const class ProportionalFont* displayFont)
{
	g_BasicRenderer->SetOrthographicProjection(Vector2::ZERO, viewDimensions);
	g_BasicRenderer->EnableDepthTesting(false);

	float cellHeight = 0.2f;
	Vector2 lineMinimums = Vector2(0.0f, viewDimensions.Y - cellHeight);

	std::string currentStateString;

	switch (m_CurrentState)
	{
	case INITIAL_STATE:
		currentStateString = "Initial State";
		break;

	case UNCONNECTED_STATE:
		currentStateString = "Unconnected State";
		break;

	case CONNECTED_STATE:
		currentStateString = "Connected State";
		break;

	case HOSTING_STATE:
		currentStateString = "Hosting State";
		break;

	case JOINING_STATE:
		currentStateString = "Joining State";
		break;
	}

	g_BasicRenderer->Draw2DProportionalText(lineMinimums, "Session: " + currentStateString, cellHeight, displayFont);
	lineMinimums.Y -= cellHeight;

	if (IsSessionRunning())
	{
		g_BasicRenderer->Draw2DProportionalText(lineMinimums, Stringf("Session Address: %s", GetSessionAddressAsString()), cellHeight, displayFont);
		lineMinimums.Y -= cellHeight;
	}

	lineMinimums.Y -= cellHeight;

	g_BasicRenderer->Draw2DProportionalText(lineMinimums, Stringf("Time elapsed since last sent: %.3fs", m_ElapsedTimeSinceLastSent), cellHeight, displayFont);
	lineMinimums.Y -= cellHeight;

	g_BasicRenderer->Draw2DProportionalText(lineMinimums, Stringf("Time elapsed since last received: %.3fs", m_ElapsedTimeSinceLastReceived), cellHeight, displayFont);
	lineMinimums.Y -= cellHeight;

	lineMinimums.Y -= cellHeight;

	if (IsSessionRunning())
	{
		g_BasicRenderer->Draw2DProportionalText(lineMinimums, Stringf("Current artificial lag: %.2f~%.2f", m_PacketChannel->m_MinimumLag, m_PacketChannel->m_MaximumLag), cellHeight, displayFont);
		lineMinimums.Y -= cellHeight;

		g_BasicRenderer->Draw2DProportionalText(lineMinimums, Stringf("Current artificial drop percentage: %.2f%%", m_PacketChannel->m_DropPercentage * 100.0), cellHeight, displayFont);
		lineMinimums.Y -= cellHeight;

		lineMinimums.Y -= cellHeight;
	}

	g_BasicRenderer->Draw2DProportionalText(lineMinimums, Stringf("Number of connections: %u", m_AllConnections.size()), cellHeight, displayFont);
	lineMinimums.Y -= cellHeight;

	for (NetConnection* currentConnection : m_AllConnections)
	{
		std::string connectionString;
		if (currentConnection->IsOwnConnection())
		{
			connectionString += "*";
		}

		if (currentConnection->IsHostConnection())
		{
			connectionString += "[Host]: ";
		}

		if (currentConnection->IsClientConnection())
		{
			connectionString += "[Client]: ";
		}

		connectionString += currentConnection->m_GlobalUniqueID;

		const char* connectionAddressString = NetworkSystem::SingletonInstance()->GetSocketAddressAsString((sockaddr*)&currentConnection->m_Address);
		connectionString += Stringf(" (%s), Index: %u, ", connectionAddressString, currentConnection->m_ConnectionIndex);

		connectionString += Stringf("Last Sent Ack: %u, Last Received Ack: %u(%u)", currentConnection->m_NextSentAck, currentConnection->m_HighestReceivedAck, currentConnection->m_PreviousReceivedAcks);

		g_BasicRenderer->Draw2DProportionalText(lineMinimums, connectionString, cellHeight, displayFont);
		lineMinimums.Y -= cellHeight;
	}
}



bool NetSession::HostSession(const char* globalUniqueID)
{
	if (IsCurrentState(HOSTING_STATE))
	{
		m_HostConnection = CreateConnection(HOST_CONNECTION_INDEX, globalUniqueID, GetSessionAddress());
		if (m_HostConnection != nullptr)
		{
			if (m_HostConnection->IsOwnConnection())
			{
				m_LocalConnection = m_HostConnection;
				m_LocalConnection->MarkAsLocalConnection();
				m_HostListening = true;
			}

			m_AllConnections.push_back(m_HostConnection);
			m_OnConnectionJoined.TriggerEvent(m_HostConnection);
			SetCurrentState(CONNECTED_STATE);

			return true;
		}
	}

	SetCurrentState(UNCONNECTED_STATE);

	return false;
}



void NetSession::JoinSession(const char* globalUniqueID, const sockaddr_in& hostAddress)
{
	if (!IsCurrentState(JOINING_STATE))
	{
		return;
	}

	NetConnection* localConnection = CreateConnection(INVALID_CONNNECTION_INDEX, globalUniqueID, GetSessionAddress());
	if (localConnection != nullptr)
	{
		if (localConnection->IsOwnConnection())
		{
			m_LocalConnection = localConnection;
			m_LocalConnection->MarkAsLocalConnection();
		}
	}

	m_HostConnection = CreateConnection(HOST_CONNECTION_INDEX, "", hostAddress);
	if (m_HostConnection != nullptr)
	{
		m_AllConnections.push_back(m_HostConnection);
	}

	NetMessage joinRequest(NET_MESSAGE_JOIN_REQUEST);
	joinRequest.WriteString(globalUniqueID);

	m_HostConnection->AddMessageToSendQueue(joinRequest);
}



void NetSession::LeaveSession()
{
	FlushToAllConnections();
	DisconnectConnection(m_LocalConnection);
	SetCurrentState(UNCONNECTED_STATE);
}



bool NetSession::ConnectConnection(NetConnection* newConnection, uint8_t connectionIndex)
{
	ASSERT_OR_DIE(!newConnection->IsConnectionConnected(), "Connection is already connected.");

	if (GetConnectionAtIndex(connectionIndex) != nullptr)
	{
		return false;
	}

	newConnection->m_ConnectionIndex = connectionIndex;
	m_AllConnections.push_back(newConnection);

	if (IsOwnConnectionConnected())
	{
		if (newConnection->IsOwnConnection())
		{
			if (!newConnection->IsHostConnection())
			{
				m_OnConnectionJoined.TriggerEvent(m_HostConnection);
			}
		}

		m_OnConnectionJoined.TriggerEvent(newConnection);

		if (newConnection->IsOwnConnection())
		{
			for (NetConnection* currentConnection : m_AllConnections)
			{
				if ((currentConnection != nullptr) && (currentConnection != newConnection) && currentConnection->IsClientConnection())
				{
					m_OnConnectionJoined.TriggerEvent(currentConnection);
				}
			}
		}
	}

	return true;
}



void NetSession::DisconnectConnection(NetConnection* correspondingConnection)
{
	if ((correspondingConnection == nullptr) || !correspondingConnection->IsConnectionConnected())
	{
		return;
	}

	uint8_t connectionIndex = correspondingConnection->m_ConnectionIndex;
	ASSERT_OR_DIE(GetConnectionAtIndex(connectionIndex) == correspondingConnection, "Connection does not exist in the list.");

	if (!IsOwnConnectionConnected())
	{
		FinalizeDisconnection(correspondingConnection);
		return;
	}

	if (correspondingConnection->IsHostConnection() && !correspondingConnection->IsOwnConnection())
	{
		DisconnectConnection(m_LocalConnection);
		return;
	}

	if (correspondingConnection->IsOwnConnection())
	{
		for (auto connectionIterator = m_AllConnections.begin(); connectionIterator != m_AllConnections.end();)
		{
			NetConnection* currentConnection = *connectionIterator;
			if ((currentConnection != nullptr) && !currentConnection->IsOwnConnection() && !currentConnection->IsHostConnection())
			{
				DisconnectConnection(currentConnection);
				break;
			}

			++connectionIterator;
		}
	}

	DeleteConnection(connectionIndex);
	m_OnConnectionLeft.TriggerEvent(correspondingConnection);

	if (correspondingConnection->IsOwnConnection() && (m_HostConnection != nullptr) && !correspondingConnection->IsHostConnection())
	{
		uint8_t hostConnectionIndex = m_HostConnection->m_ConnectionIndex;
		DeleteConnection(hostConnectionIndex);
		m_OnConnectionLeft.TriggerEvent(m_HostConnection);
		FinalizeDisconnection(m_HostConnection);

		ASSERT_OR_DIE(m_HostConnection == nullptr, "Host has not been disconnected.");
	}

	FinalizeDisconnection(correspondingConnection);
}



void NetSession::FlushToAllConnections()
{
	NetMessage leaveMessage(NET_MESSAGE_LEAVE);
	for (NetConnection* currentConnection : m_AllConnections)
	{
		if (!currentConnection->IsOwnConnection())
		{
			currentConnection->AddMessageToSendQueue(leaveMessage);
			currentConnection->SendPacketToMyConnection();
		}
	}
}



NetConnection* NetSession::CreateConnection(uint8_t connectionIndex, const char* globalUniqueID, const sockaddr_in& connectionAddress)
{
	if (GetConnectionAtIndex(connectionIndex) != nullptr || GetConnectionWithName(globalUniqueID) != nullptr)
	{
		return nullptr;
	}

	NetConnection* newConnection = NetConnection::CreateNetConnection(this, connectionIndex, connectionAddress, globalUniqueID);

	return newConnection;
}



void NetSession::DeleteConnection(uint8_t connectionIndex)
{
	NetConnection* destroyableConnection = GetConnectionAtIndex(connectionIndex);
	if (destroyableConnection == nullptr)
	{
		return;
	}

	for (auto connectionIterator = m_AllConnections.begin(); connectionIterator != m_AllConnections.end();)
	{
		NetConnection* currentConnection = *connectionIterator;
		if (currentConnection == destroyableConnection)
		{
			m_AllConnections.erase(connectionIterator);
			break;
		}

		++connectionIterator;
	}
}



void NetSession::SendDirectMessage(const sockaddr_in& toAddress, NetMessage& currentMessage)
{
	if (!IsSessionRunning())
	{
		return;
	}

	PacketHeader packetHeader;
	packetHeader.m_SenderConnectionIndex = (GetLocalConnection() == nullptr) ? INVALID_CONNNECTION_INDEX : GetLocalConnectionIndex();
	packetHeader.m_PacketAck = INVALID_PACKET_ACK;
	packetHeader.m_MostRecentAck = INVALID_PACKET_ACK;
	packetHeader.m_PreviouslyReceivedAcksBitfield = 0;

	NetPacket packetToSend;
	packetToSend.WritePacketHeader(packetHeader);
	packetToSend.Write<uint8_t>(1U);

	currentMessage.m_MessageDefinition = FindMessageDefinition(currentMessage.m_MessageID);
	if (currentMessage.m_MessageDefinition != nullptr)
	{
		if (packetToSend.CanWriteMessage(&currentMessage))
		{
			packetToSend.WriteMessage(&currentMessage);
		}

		m_PacketChannel->SendToAddressOnPacketChannel(packetToSend.m_Buffer, packetToSend.GetTotalReadableSize(), toAddress);
	}
}



void NetSession::RegisterMessage(uint8_t messageID, uint8_t controlFlags, uint8_t optionFlags, NetMessageCallBack* messageCallBack)
{
	if (IsSessionRunning())
	{
		return;
	}

	if (m_CurrentState != INITIAL_STATE)
	{
		return;
	}
	
	NetMessageDefinition messageDefinition = NetMessageDefinition(messageID, controlFlags, optionFlags, messageCallBack);

	if (FindMessageDefinition(messageID) == nullptr)
	{
		AddMessageDefinition(messageID, messageDefinition);
	}
}



void NetSession::SendToHostConnection(NetMessage& currentMessage)
{
	if (m_HostConnection != nullptr)
	{
		m_HostConnection->AddMessageToSendQueue(currentMessage);
	}
}



void NetSession::SendToAllConnections(NetMessage& currentMessage)
{
	for (NetConnection* currentConnection : m_AllConnections)
	{
		if (!currentConnection->IsOwnConnection() || currentConnection->IsHostConnection())
		{
			currentConnection->AddMessageToSendQueue(currentMessage);
		}
	}
}



const sockaddr_in NetSession::GetSessionAddress() const
{
	return m_PacketChannel->m_UDPSocket->m_Address;
}



const char* NetSession::GetSessionAddressAsString() const
{
	return NetworkSystem::SingletonInstance()->GetSocketAddressAsString((sockaddr*)(&m_PacketChannel->m_UDPSocket->m_Address));
}



bool NetSession::IsSessionRunning() const
{
	if (m_PacketChannel != nullptr)
	{
		if (m_PacketChannel->IsChannelActive())
		{
			return true;
		}
	}

	return false;
}



void NetSession::SetListening(bool listening)
{
	if (IsOwnConnectionHost())
	{
		m_HostListening = listening;
	}
}



bool NetSession::IsHostListening() const
{
	if (IsOwnConnectionHost())
	{
		return m_HostListening;
	}

	return false;
}



NetConnection* NetSession::GetHostConnection() const
{
	return m_HostConnection;
}



NetConnection* NetSession::GetLocalConnection() const
{
	return m_LocalConnection;
}



uint8_t NetSession::GetHostConnectionIndex() const
{
	return GetHostConnection()->m_ConnectionIndex;
}



uint8_t NetSession::GetLocalConnectionIndex() const
{
	return GetLocalConnection()->m_ConnectionIndex;
}



bool NetSession::IsOwnConnectionHost() const
{
	if (m_LocalConnection != nullptr && m_HostConnection != nullptr)
	{
		if (m_HostConnection == m_LocalConnection)
		{
			return true;
		}
	}
	
	return false;
}



bool NetSession::IsGlobalUniqueIDAvailable(const char* globalUniqueID) const
{
	return (GetConnectionWithName(globalUniqueID) == nullptr);
}



bool NetSession::IsSessionFull() const
{
	return (m_AllConnections.size() == MAXIMUM_NUMBER_OF_CONNECTIONS);
}



void NetSession::SetCurrentState(const NetSessionState& currentState)
{
	m_CurrentState = currentState;
}



NetSessionState NetSession::GetCurrentState() const
{
	return m_CurrentState;
}



bool NetSession::IsCurrentState(const NetSessionState& currentState) const
{
	return (m_CurrentState == currentState);
}



NetConnection* NetSession::GetConnectionAtIndex(uint8_t connectionIndex) const
{
	for (NetConnection* currentConnection : m_AllConnections)
	{
		if (currentConnection->m_ConnectionIndex == connectionIndex)
		{
			return currentConnection;
		}
	}

	return nullptr;
}



NetConnection* NetSession::GetConnectionWithName(const char* globalUniqueID) const
{
	for (NetConnection* currentConnection : m_AllConnections)
	{
		if (strcmp(currentConnection->m_GlobalUniqueID, globalUniqueID) == 0)
		{
			return currentConnection;
		}
	}

	return nullptr;
}



NetConnection* NetSession::GetConnectionFromAddress(const sockaddr_in& connectionAddress) const
{
	for (NetConnection* currentConnection : m_AllConnections)
	{
		if (NetworkSystem::SingletonInstance()->AreAddressesSame(currentConnection->m_Address, connectionAddress))
		{
			return currentConnection;
		}
	}

	return nullptr;
}



NetMessageDefinition* NetSession::FindMessageDefinition(uint8_t messageID)
{
	NetMessageDefinition& messageDefinition = m_AllMessageDefinitions[messageID];
	if (messageDefinition.m_MessageID != INVALID_MESSAGE_TYPE && messageDefinition.m_ControlFlags != INVALID_CONTROL_TYPE && messageDefinition.m_OptionFlags != INVALID_OPTION_TYPE && messageDefinition.m_MessageCallBack != nullptr)
	{
		return &messageDefinition;
	}

	return nullptr;
}



void NetSession::AddMessageDefinition(uint8_t messageID, const NetMessageDefinition& messageDefinition)
{
	ASSERT_OR_DIE(messageID <= NUMBER_OF_MESSAGE_DEFINITIONS, "Message ID beyond range.");

	m_AllMessageDefinitions[messageID] = messageDefinition;
}



void NetSession::WriteConnectionInfo(NetMessage& currentMessage, const NetConnectionInfo& currentConnectionInfo)
{
	currentMessage.Write<uint8_t>(currentConnectionInfo.m_ConnectionIndex);
	currentMessage.Write<USHORT>(currentConnectionInfo.m_Address.sin_port);
	currentMessage.Write<ULONG>(currentConnectionInfo.m_Address.sin_addr.S_un.S_addr);
	currentMessage.WriteString(currentConnectionInfo.m_GlobalUniqueID);
}



void NetSession::ReadConnectionInfo(const NetMessage& currentMessage, NetConnectionInfo& currentConnectionInfo)
{
	currentMessage.Read<uint8_t>(&currentConnectionInfo.m_ConnectionIndex);
	currentMessage.Read<USHORT>(&currentConnectionInfo.m_Address.sin_port);
	currentMessage.Read<ULONG>(&currentConnectionInfo.m_Address.sin_addr.S_un.S_addr);
	CopyString(currentConnectionInfo.m_GlobalUniqueID, currentMessage.ReadString(), MAXIMUM_GUID_LENGTH);
}



void NetSession::UpdateAndPrintLatestError(uint8_t latestError)
{
	m_LatestError = latestError;

	switch (m_LatestError)
	{
	case NO_ERROR_CODE:
		break;

	case START_ERROR_FAILED_TO_CREATE_SOCKET:
	{
		ConsoleLine errorMessage("Error: START_ERROR_FAILED_TO_CREATE_SOCKET", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(errorMessage);
	}
	break;

	case JOIN_ERROR_HOST_TIMED_OUT:
	{
		ConsoleLine errorMessage("Error: JOIN_ERROR_HOST_TIMED_OUT", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(errorMessage);
	}
	break;

	case JOIN_DENIED_HOST_NOT_LISTENING:
	{
		ConsoleLine errorMessage("Error: JOIN_DENIED_HOST_NOT_LISTENING", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(errorMessage);
	}
	break;

	case JOIN_DENIED_NOT_HOST:
	{
		ConsoleLine errorMessage("Error: JOIN_DENIED_NOT_HOST", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(errorMessage);
	}
	break;

	case JOIN_DENIED_FULL:
	{
		ConsoleLine errorMessage("Error: JOIN_DENIED_FULL", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(errorMessage);
	}
	break;

	case JOIN_DENIED_GUID_IN_USE:
	{
		ConsoleLine errorMessage("Error: JOIN_DENIED_GUID_IN_USE", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(errorMessage);
	}
	break;

	case ERROR_HOST_DISCONNECTED:
	{
		ConsoleLine errorMessage("Error: ERROR_HOST_DISCONNECTED", RGBA::RED);
		DeveloperConsole::AddNewConsoleLine(errorMessage);
	}
	break;

	default:
		break;
	}
}



void NetSession::SendDenyMessage(uint8_t errorCode, const sockaddr_in& toAddress)
{
	NetMessage denyMessage(NET_MESSAGE_JOIN_DENY);
	denyMessage.Write<uint8_t>(errorCode);

	SendDirectMessage(toAddress, denyMessage);
}



NetConnection* NetSession::AddNewConnection(const char* globalUniqueID, const sockaddr_in& connectionAddress)
{
	uint8_t connectionIndex = HOST_CONNECTION_INDEX;
	while (GetConnectionAtIndex(connectionIndex) != nullptr)
	{
		++connectionIndex;
	}

	NetConnection* newConnection = CreateConnection(INVALID_CONNNECTION_INDEX, globalUniqueID, connectionAddress);
	ConnectConnection(newConnection, connectionIndex);

	return newConnection;
}



void NetSession::ProcessIncomingPackets()
{
	NetPacket receivedPacket;

	NetSender fromSender;
	fromSender.m_Session = this;

	while (ReadNextPacketFromSocket(receivedPacket, fromSender.m_Address))
	{
		PacketHeader packetHeader;
		ProcessPacketHeader(packetHeader, receivedPacket, fromSender);

		uint8_t numberOfMessages;
		receivedPacket.Read<uint8_t>(&numberOfMessages);

		NetMessage currentMessage;
		while (numberOfMessages > 0)
		{
			--numberOfMessages;
			ProcessNextMessageFromPacket(receivedPacket, currentMessage, fromSender);
		}

		if (fromSender.m_Connection != nullptr)
		{
			fromSender.m_Connection->MarkPacketReceived(packetHeader);
		}
	}
}



void NetSession::UpdateAllSessionStates()
{
	switch (m_CurrentState)
	{
	case INITIAL_STATE:
		break;

	case UNCONNECTED_STATE:
		UpdateAllConnections();
		break;

	case CONNECTED_STATE:
		UpdateAllConnections();
		break;

	case HOSTING_STATE:
		break;

	case JOINING_STATE:
		TimeoutHostConnection();
		UpdateAllConnections();
		break;
	}
}



void NetSession::UpdateAllConnections()
{
	m_ElapsedTime += Clock::MasterClock()->GetDeltaTimeFloat();
	if (m_ElapsedTime >= UPDATE_RATE)
	{
		for (NetConnection* currentConnection : m_AllConnections)
		{
			if(!currentConnection->IsOwnConnection() || currentConnection->IsHostConnection())
			{
				m_OnConnectionUpdated.TriggerEvent(currentConnection);
				currentConnection->SendPacketToMyConnection();
			}
		}

		m_ElapsedTime = 0.0f;
	}
}



void NetSession::TimeoutHostConnection()
{
	m_HostTimeOut += Clock::MasterClock()->GetDeltaTimeFloat();
	if (m_HostTimeOut >= TIMEOUT_THRESHOLD)
	{
		LeaveSession();
		m_HostTimeOut = 0.0f;
		UpdateAndPrintLatestError(JOIN_ERROR_HOST_TIMED_OUT);
	}
}



bool NetSession::ReadNextPacketFromSocket(NetPacket& nextPacket, sockaddr_in& fromAddress)
{
	size_t receivedSize = m_PacketChannel->ReceiveFromAddressOnPacketChannel(nextPacket.m_Buffer, PACKET_MTU, &fromAddress);
	if (receivedSize > 0)
	{
		m_ElapsedTimeSinceLastReceived = 0.0f;
		nextPacket.SetReadableSize(receivedSize);
		nextPacket.ResetOffset();
		return true;
	}

	return false;
}



void NetSession::ProcessPacketHeader(PacketHeader& packetHeader, NetPacket& receivedPacket, NetSender& fromSender)
{
	receivedPacket.ReadPacketHeader(packetHeader);

	fromSender.m_Connection = GetConnectionAtIndex(packetHeader.m_SenderConnectionIndex);
	if (fromSender.m_Connection != nullptr)
	{
		if (!fromSender.m_Connection->IsConnectionConfirmed())
		{
			fromSender.m_Connection->MarkConnectionConfirmed();
		}
	}
}



void NetSession::ProcessNextMessageFromPacket(NetPacket& receivedPacket, NetMessage& currentMessage, NetSender& fromSender)
{
	receivedPacket.ReadMessage(&currentMessage);
	if (MessageCanBeProcessed(fromSender, currentMessage))
	{
		if (fromSender.m_Connection != nullptr)
		{
			fromSender.m_Connection->ProcessMessage(fromSender, currentMessage);
		}
		else
		{
			currentMessage.ProcessMessage(fromSender);
			if (currentMessage.IsReliable())
			{
				fromSender.m_Connection = GetConnectionFromAddress(fromSender.m_Address);
			}
		}

		if (fromSender.m_Connection != nullptr)
		{

			fromSender.m_Connection->MarkMessageReceived(currentMessage);
			
			if (!fromSender.m_Connection->IsConnectionConfirmed())
			{
				fromSender.m_Connection->MarkConnectionConfirmed();
			}
		}
	}

	currentMessage.ResetOffset();
}



bool NetSession::MessageCanBeProcessed(const NetSender& fromSender, const NetMessage& currentMessage)
{
	if (fromSender.m_Connection == nullptr)
	{
		return !currentMessage.RequiresConnection();
	}
	else
	{
		if (currentMessage.IsReliable())
		{
			return !fromSender.m_Connection->HasReceivedReliableID(currentMessage.m_ReliableID);
		}
		else
		{
			return true;
		}
	}
}



bool NetSession::IsOwnConnectionConnected() const
{
	if (m_LocalConnection != nullptr)
	{
		if (m_LocalConnection->IsConnectionConnected())
		{
			return true;
		}
	}

	return false;
}



void NetSession::FinalizeDisconnection(NetConnection*& currentConnection)
{
	DeleteConnection(currentConnection->m_ConnectionIndex);
	NetConnection::DestroyNetConnection(currentConnection);
}



void OnPingReceived(const NetSender& pingSender, const NetMessage& pingMessage)
{
	const char* messageString = pingMessage.ReadString();
	const char* senderAddress = NetworkSystem::SingletonInstance()->GetSocketAddressAsString((sockaddr*)&pingSender.m_Address);
	ConsoleLine pingConsoleMessage = ConsoleLine(Stringf("Ping received from %s. [%s]", senderAddress, ((messageString != nullptr) ? messageString : "Null")), RGBA::GREEN);

	NetMessage pongMessage(NET_MESSAGE_PONG);
	NetSession::LocalNetSession()->SendDirectMessage(pingSender.m_Address, pongMessage);
	
	DeveloperConsole::AddNewConsoleLine(pingConsoleMessage);
}



void OnPongReceived(const NetSender& pongSender, const NetMessage& pongMessage)
{
	pongMessage;
	const char* senderAddress = NetworkSystem::SingletonInstance()->GetSocketAddressAsString((sockaddr*)&pongSender.m_Address);
	ConsoleLine pongConsoleMessage = ConsoleLine(Stringf("Pong received from %s.", senderAddress), RGBA::GREEN);

	DeveloperConsole::AddNewConsoleLine(pongConsoleMessage);
}



void OnJoinRequested(const NetSender& requestSender, const NetMessage& requestMessage)
{
	NetSession* currentSession = requestSender.m_Session;
	const char* globalUniqueID = requestMessage.ReadString();

	if (!currentSession->IsOwnConnectionHost())
	{
		currentSession->SendDenyMessage(JOIN_DENIED_NOT_HOST, requestSender.m_Address);
		return;
	}

	if (!currentSession->IsHostListening())
	{
		currentSession->SendDenyMessage(JOIN_DENIED_HOST_NOT_LISTENING, requestSender.m_Address);
		return;
	}

	if (currentSession->IsSessionFull())
	{
		currentSession->SendDenyMessage(JOIN_DENIED_FULL, requestSender.m_Address);
		return;
	}

	if (!currentSession->IsGlobalUniqueIDAvailable(globalUniqueID))
	{
		currentSession->SendDenyMessage(JOIN_DENIED_GUID_IN_USE, requestSender.m_Address);
		return;
	}

	currentSession->AddNewConnection(globalUniqueID, requestSender.m_Address);
}



void OnJoinDenied(const NetSender& denySender, const NetMessage& denyMessage)
{
	NetSession* currentSession = denySender.m_Session;

	if (!currentSession->IsCurrentState(JOINING_STATE))
	{
		return;
	}

	currentSession->LeaveSession();

	uint8_t latestError;
	denyMessage.Read<uint8_t>(&latestError);
	currentSession->UpdateAndPrintLatestError(latestError);
}



void OnJoinAccepted(const NetSender& acceptSender, const NetMessage& acceptMessage)
{
	NetSession* currentSession = acceptSender.m_Session;
	
	if (!currentSession->IsCurrentState(JOINING_STATE))
	{
		return;
	}

	NetConnectionInfo hostConnectionInfo;
	NetConnectionInfo localConnectionInfo;
	currentSession->ReadConnectionInfo(acceptMessage, hostConnectionInfo);
	currentSession->ReadConnectionInfo(acceptMessage, localConnectionInfo);

	NetConnection* hostConnection = currentSession->GetHostConnection();
	hostConnection->UpdateConnectionInfo(hostConnectionInfo);

	NetConnection* localConnection = currentSession->GetLocalConnection();
	if (currentSession->ConnectConnection(localConnection, localConnectionInfo.m_ConnectionIndex))
	{
		currentSession->SetCurrentState(CONNECTED_STATE);
	}
}



void OnSessionLeft(const NetSender& leaveSender, const NetMessage& leaveMessage)
{
	leaveMessage;
	NetSession* currentSession = leaveSender.m_Session;
	
	NetConnection* leftConnection = leaveSender.m_Connection;
	if (leftConnection == nullptr)
	{
		return;
	}

	currentSession->DisconnectConnection(leftConnection);
}



void StartSessionCommand(Command& currentCommand)
{
	ConsoleLine startMessage;
	
	if (currentCommand.HasNoArguments())
	{
		if (NetSession::LocalNetSession() != nullptr)
		{
			if (!NetSession::LocalNetSession()->IsSessionRunning())
			{
				uint16_t servicePortNumber = STARTING_PORT_NUMBER;
				while (!NetSession::LocalNetSession()->IsSessionRunning())
				{
					if (servicePortNumber > STARTING_PORT_NUMBER + PORT_RANGE)
					{
						break;
					}

					std::string servicePort = Stringf("%u", servicePortNumber);
					NetSession::LocalNetSession()->StartSessionOnPort(servicePort.c_str());

					++servicePortNumber;
				}

				if (NetSession::LocalNetSession()->IsSessionRunning())
				{
					NetSession::LocalNetSession()->SetCurrentState(UNCONNECTED_STATE);
					startMessage = ConsoleLine(Stringf("Session started and running on %s", NetSession::LocalNetSession()->GetSessionAddressAsString()), RGBA::GREEN);
				}
				else
				{
					startMessage = ConsoleLine("Session failed to start.", RGBA::RED);
					NetSession::LocalNetSession()->UpdateAndPrintLatestError(START_ERROR_FAILED_TO_CREATE_SOCKET);
				}
			}
			else
			{
				startMessage = ConsoleLine("Session is already running.", RGBA::RED);
			}
		}
		else
		{
			startMessage = ConsoleLine("No session currently exists.", RGBA::RED);
		}
	}
	else
	{
		if (NetSession::LocalNetSession() != nullptr)
		{
			if (!NetSession::LocalNetSession()->IsSessionRunning())
			{
				std::vector<std::string> currentCommandArguments;
				currentCommand.GetCommandArguments(currentCommandArguments);
				const char* servicePort = currentCommandArguments[0].c_str();

				NetSession::LocalNetSession()->StartSessionOnPort(servicePort);

				if (NetSession::LocalNetSession()->IsSessionRunning())
				{
					NetSession::LocalNetSession()->SetCurrentState(UNCONNECTED_STATE);
					startMessage = ConsoleLine(Stringf("Session started and running on %s", NetSession::LocalNetSession()->GetSessionAddressAsString()), RGBA::GREEN);
				}
				else
				{
					startMessage = ConsoleLine("Session failed to start.", RGBA::RED);
					NetSession::LocalNetSession()->UpdateAndPrintLatestError(START_ERROR_FAILED_TO_CREATE_SOCKET);
				}
			}
			else
			{
				startMessage = ConsoleLine("Session is already running.", RGBA::RED);
			}
		}
		else
		{
			startMessage = ConsoleLine("No session currently exists.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(startMessage);
}



void StopSessionCommand(Command& currentCommand)
{
	ConsoleLine stopMessage;

	if (currentCommand.HasNoArguments())
	{
		if (NetSession::LocalNetSession()->IsSessionRunning() && NetSession::LocalNetSession()->IsCurrentState(UNCONNECTED_STATE))
		{
			PacketChannel::DestroyPacketChannel(NetSession::LocalNetSession()->m_PacketChannel);
			if (!NetSession::LocalNetSession()->IsSessionRunning())
			{
				NetSession::LocalNetSession()->SetCurrentState(INITIAL_STATE);
				stopMessage = ConsoleLine("Session stopped.", RGBA::GREEN);
			}
			else
			{
				stopMessage = ConsoleLine("Failed to stop session.", RGBA::RED);
			}
		}
		else
		{
			stopMessage = ConsoleLine("Cannot stop session.", RGBA::RED);
		}
	}
	else
	{
		stopMessage = ConsoleLine("Stop Session command takes no arguments.", RGBA::RED);
	}

	DeveloperConsole::AddNewConsoleLine(stopMessage);
}



void PingMessageCommand(Command& currentCommand)
{
	ConsoleLine pingConsoleMessage;
	
	if (currentCommand.HasNoArguments())
	{
		pingConsoleMessage = ConsoleLine("Cannot send ping. No address provided.", RGBA::RED);
	}
	else
	{
		std::vector<std::string> currentCommandArguments;
		currentCommand.GetCommandArguments(currentCommandArguments);

		if (currentCommandArguments.size() >= 2)
		{
			if (NetSession::LocalNetSession()->IsSessionRunning())
			{
				const char* ipAddress = currentCommandArguments[0].c_str();
				int servicePort = atoi(currentCommandArguments[1].c_str());

				sockaddr_in toAddress = NetworkSystem::SingletonInstance()->GetSocketAddressFromString(ipAddress, static_cast<uint16_t>(servicePort));

				NetMessage pingMessage(NET_MESSAGE_PING);
				const char* optionalMessage = nullptr;

				for (size_t currentMessageIndex = 2; currentMessageIndex < currentCommandArguments.size(); ++currentMessageIndex)
				{
					optionalMessage = currentCommandArguments[currentMessageIndex].c_str();
					pingMessage.WriteString(optionalMessage);

					optionalMessage = nullptr;
				}

				NetSession::LocalNetSession()->SendDirectMessage(toAddress, pingMessage);

				pingConsoleMessage = ConsoleLine("Ping sent to " + currentCommandArguments[0] + ":" + currentCommandArguments[1], RGBA::GREEN);
			}
			else
			{
				pingConsoleMessage = ConsoleLine("Cannot send ping. No session running.", RGBA::RED);
			}
		}
		else
		{
			pingConsoleMessage = ConsoleLine("Ping error. Number of arguments provided does not match.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(pingConsoleMessage);
}



void HostSessionCommand(Command& currentCommand)
{
	ConsoleLine hostMessage;

	if (currentCommand.HasNoArguments())
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			hostMessage = ConsoleLine("No arguments given. Host command needs a valid User ID as argument.", RGBA::RED);
		}
		else
		{
			hostMessage = ConsoleLine("Cannot host session. No session currently exists.", RGBA::RED);
		}
	}
	else
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			if (NetSession::LocalNetSession()->IsCurrentState(UNCONNECTED_STATE))
			{
				std::vector<std::string> currentCommandArguments;
				currentCommand.GetCommandArguments(currentCommandArguments);
				const char* globalUniqueID = currentCommandArguments[0].c_str();

				NetSession::LocalNetSession()->SetCurrentState(HOSTING_STATE);
				if (NetSession::LocalNetSession()->HostSession(globalUniqueID))
				{
					hostMessage = ConsoleLine(Stringf("Session on %s currently hosted with User ID %s.", NetSession::LocalNetSession()->GetSessionAddressAsString(), globalUniqueID), RGBA::GREEN);
				}
				else
				{
					hostMessage = ConsoleLine("Failed to host session. Host connection or User ID exists.", RGBA::RED);
				}
			}
			else
			{
				hostMessage = ConsoleLine("Unable to host session. Session is currently not in unconnected state.", RGBA::RED);
			}
		}
		else
		{
			hostMessage = ConsoleLine("Cannot host session. No session currently exists.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(hostMessage);
}



void JoinSessionCommand(Command& currentCommand)
{
	ConsoleLine joinMessage;

	if (currentCommand.HasNoArguments())
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			joinMessage = ConsoleLine("No arguments given. Join command needs valid User ID, Host IP address and Host service port as arguments.", RGBA::RED);
		}
		else
		{
			joinMessage = ConsoleLine("Cannot join from session. No session currently exists.", RGBA::RED);
		}
	}
	else
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			std::vector<std::string> currentCommandArguments;
			currentCommand.GetCommandArguments(currentCommandArguments);

			if (currentCommandArguments.size() == 3)
			{
				if (NetSession::LocalNetSession()->IsCurrentState(UNCONNECTED_STATE))
				{
					const char* globalUniqueID = currentCommandArguments[0].c_str();
					const char* ipAddress = currentCommandArguments[1].c_str();
					uint16_t portNumber = static_cast<uint16_t>(stoi(currentCommandArguments[2]));
					sockaddr_in connectionAddress = NetworkSystem::SingletonInstance()->GetSocketAddressFromString(ipAddress, portNumber);

					NetSession::LocalNetSession()->SetCurrentState(JOINING_STATE);
					NetSession::LocalNetSession()->JoinSession(globalUniqueID, connectionAddress);

					joinMessage = ConsoleLine("Join request sent.", RGBA::GREEN);
				}
				else
				{
					joinMessage = ConsoleLine("Unable to join from session. Session is currently not in unconnected state.", RGBA::RED);
				}
			}
			else
			{
				joinMessage = ConsoleLine("Number of arguments provided does not match.", RGBA::RED);
			}
		}
		else
		{
			joinMessage = ConsoleLine("Cannot join from session. No session currently exists.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(joinMessage);
}



void LeaveSessionCommand(Command& currentCommand)
{
	ConsoleLine leaveMessage;

	if (currentCommand.HasNoArguments())
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			if(NetSession::LocalNetSession()->IsCurrentState(CONNECTED_STATE))
			{
				NetSession::LocalNetSession()->LeaveSession();
				if (NetSession::LocalNetSession()->IsCurrentState(UNCONNECTED_STATE))
				{
					leaveMessage = ConsoleLine("Successfully left current session.", RGBA::GREEN);
				}
				else
				{
					leaveMessage = ConsoleLine("Failed to leave session. Session was not in connected state.", RGBA::RED);
				}
			}
			else
			{
				leaveMessage = ConsoleLine("Failed to leave session. Session was not in connected state.", RGBA::RED);
			}
		}
		else
		{
			leaveMessage = ConsoleLine("No session currently exists.", RGBA::RED);
		}
	}
	else
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			leaveMessage = ConsoleLine("Leave command does not take any arguments.", RGBA::RED);
		}
		else
		{
			leaveMessage = ConsoleLine("No session currently exists.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(leaveMessage);
}



void ListConnectionsCommand(Command& currentCommand)
{
	ConsoleLine listMessage;

	if (currentCommand.HasNoArguments())
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			for (NetConnection* currentConnection : NetSession::LocalNetSession()->m_AllConnections)
			{
				const char* connectionAddress = NetworkSystem::SingletonInstance()->GetSocketAddressAsString((sockaddr*)&currentConnection->m_Address);
				uint8_t connectionIndex = currentConnection->m_ConnectionIndex;
				const char* connectionID = currentConnection->m_GlobalUniqueID;

				ConsoleLine connectionMessage = ConsoleLine(Stringf("Connection: %u %s %s", connectionIndex, connectionID, connectionAddress), RGBA::YELLOW);
				DeveloperConsole::AddNewConsoleLine(connectionMessage);
			}

			listMessage = ConsoleLine("Listing all connected connections.", RGBA::GREEN);
		}
		else
		{
			listMessage = ConsoleLine("Cannot run command. No session is currently running.", RGBA::RED);
		}
	}
	else
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			listMessage = ConsoleLine("List Connections command takes no arguments.", RGBA::RED);
		}
		else
		{
			listMessage = ConsoleLine("Cannot run command. No session is currently running.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(listMessage);
}



void SimulateLagCommand(Command& currentCommand)
{
	ConsoleLine lagMessage;
	
	if (currentCommand.HasNoArguments())
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			lagMessage = ConsoleLine("No arguments given. SimulateNetLag takes one argument for constant and two arguments for range.", RGBA::RED);
		}
		else
		{
			lagMessage = ConsoleLine("Cannot run command. No session is currently running.", RGBA::RED);
		}
	}
	else
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			std::vector<std::string> currentCommandArguments;
			currentCommand.GetCommandArguments(currentCommandArguments);

			if (currentCommandArguments.size() > 2)
			{
				lagMessage = ConsoleLine("Cannot process command. SimulateNetLag doesn't take more than two arguments.", RGBA::RED);
			}
			else
			{
				if (currentCommandArguments.size() == 1)
				{
					double lagAmount = stod(currentCommandArguments[0]);
					NetSession::LocalNetSession()->m_PacketChannel->m_MinimumLag = lagAmount;
					NetSession::LocalNetSession()->m_PacketChannel->m_MaximumLag = lagAmount;

					lagMessage = ConsoleLine("Lag set to constant amount.", RGBA::GREEN);
				}
				else if (currentCommandArguments.size() == 2)
				{
					double lagMinimum = stod(currentCommandArguments[0]);
					double lagMaximum = stod(currentCommandArguments[1]);

					NetSession::LocalNetSession()->m_PacketChannel->m_MinimumLag = lagMinimum;
					NetSession::LocalNetSession()->m_PacketChannel->m_MaximumLag = lagMaximum;

					lagMessage = ConsoleLine("Lag set to range.", RGBA::GREEN);
				}
			}
		}
		else
		{
			lagMessage = ConsoleLine("Cannot run command. No session is currently running.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(lagMessage);
}



void SimulateLossCommand(Command& currentCommand)
{
	ConsoleLine lossMessage;
	
	if (currentCommand.HasNoArguments())
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			lossMessage = ConsoleLine("No arguments given. SimulateNetLoss takes one argument.", RGBA::RED);
		}
		else
		{
			lossMessage = ConsoleLine("Cannot run command. No session is currently running.", RGBA::RED);
		}
	}
	else
	{
		if (NetSession::LocalNetSession()->IsSessionRunning())
		{
			std::vector<std::string> currentCommandArguments;
			currentCommand.GetCommandArguments(currentCommandArguments);

			if (currentCommandArguments.size() > 1)
			{
				lossMessage = ConsoleLine("Too many arguments. SimulateNetLoss takes exactly one argument.", RGBA::RED);
			}
			else
			{
				float lossPercentage = stof(currentCommandArguments[0]) * 0.01f;
				NetSession::LocalNetSession()->m_PacketChannel->m_DropPercentage = lossPercentage;

				lossMessage = ConsoleLine("Loss percentage set.", RGBA::GREEN);
			}
		}
		else
		{
			lossMessage = ConsoleLine("Cannot run command. No session is currently running.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(lossMessage);
}