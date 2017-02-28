#include "gaga_server.h"
#include "gaga_timer.h"
#include "gaga_util.h"

#include "GameMsg.pb.h"
using namespace proto;

static GagaServer *g_gagaServerInstance = nullptr;
GagaServer* GagaServer::GetInstance() {
	if (nullptr == g_gagaServerInstance) {
		g_gagaServerInstance = new GagaServer();
	}
	return g_gagaServerInstance;
}

bool GagaServer::Start() {
	char *password = "fuck";
	int passwordLength = 4;
	unsigned short port = 8989;
	unsigned short maxClients = 10;

	// init cliet pool
	m_clientAgentPool = new ObjectPool<ClientAgent>(maxClients);
	m_roomPool = new ObjectPool<Room>(maxClients);
	m_playerPool = new ObjectPool<Player>(maxClients);

	m_rakPeer = RakNet::RakPeerInterface::GetInstance();
	m_rakPeer->SetTimeoutTime(10000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	m_rakPeer->SetIncomingPassword(password, passwordLength);
	m_rakPeer->SetMaximumIncomingConnections(maxClients);
	m_rakPeer->SetOccasionalPing(true);
	m_rakPeer->SetUnreliableTimeout(1000);
	
	RakNet::SocketDescriptor socketDescriptors[2];
	socketDescriptors[0].port = port;
	socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4
	socketDescriptors[1].port = port;
	socketDescriptors[1].socketFamily = AF_INET6; // Test out IPV6

	bool bOk = m_rakPeer->Startup(maxClients, socketDescriptors, 2) == RakNet::RAKNET_STARTED;
	if (!bOk) {
		gaga_printf("Failed to start dual IPV4 and IPV6 ports. Trying IPV4 only.");
		bool bOk = m_rakPeer->Startup(maxClients, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
		if (!bOk) {
			gaga_printf("Server failed to start.  Terminating.");
			return false;
		}
		else {
			gaga_printf("IPV4 only success.");
		}
	}

	// Do Test Case
	TestCase();
	
	return true;
}

void GagaServer::Stop() {
	RakNet::RakPeerInterface::DestroyInstance(m_rakPeer);
}

void GagaServer::Tick() {
	RakNet::Packet *packet = nullptr;
	TimerManager::GetInstance()->Tick();
	for (packet = m_rakPeer->Receive(); packet; m_rakPeer->DeallocatePacket(packet), packet = m_rakPeer->Receive()) {
		HandlePacket(packet);
	}
}

void GagaServer::HandlePacket(RakNet::Packet *packet) {
	RakNet::MessageID messageId = (unsigned char)(packet->data[0]);
	if (messageId == ID_NEW_INCOMING_CONNECTION) {
		gaga_printf("New Client Connnect..");
		RakNet::RakNetGUID guid = packet->guid;
		{
			ClientAgent *clientAgent = FindClientAgent(guid);
			if (nullptr == clientAgent) {
				clientAgent = AddClientAgent(guid);
				if (clientAgent) {
					clientAgent->set_guid(guid);
					clientAgent->set_addr(packet->systemAddress);
				}
			}
		}
	}
	else if (messageId == ID_DETECT_LOST_CONNECTIONS) {
		RakNet::RakNetGUID guid = packet->guid;
	}
	else if (messageId >= proto::MSG_RESERVE) {
		RakNet::RakNetGUID guid = packet->guid;
		gaga_printf("guid %s\n", guid.ToString());
		ClientAgent* clientAgent = FindClientAgent(guid);
		if (clientAgent) {
			clientAgent->HandlePacket(packet);
		}
		else {
			gaga_printf("No such client agnet");
		}
	}
}

ClientAgent* GagaServer::FindClientAgent(const RakNet::RakNetGUID& guid) {
	auto search = m_onlineClientList.find(guid);
	if (search != m_onlineClientList.end()) {
		return search->second;
	}
	return nullptr;
}

ClientAgent* GagaServer::AddClientAgent(const RakNet::RakNetGUID& guid) {
	ClientAgent *clientAgent = m_clientAgentPool->AcquireObject();
	if (clientAgent) {
		m_onlineClientList.insert(std::make_pair(guid, clientAgent));
	}
	return clientAgent;
}

void GagaServer::RemoveClientAgent(const RakNet::RakNetGUID& guid) {
	ClientAgent *clientAgent = FindClientAgent(guid);
	if (clientAgent) {
		m_onlineClientList.erase(guid);
		m_clientAgentPool->ReleaseObject(clientAgent);
	}
}

void GagaServer::AddGUIDMap(const std::string userid, const RakNet::RakNetGUID guid) {
	m_userid2guidMap.insert(std::make_pair(userid, guid));
}

void GagaServer::RemoveGUIDMap(const std::string userid) {
	m_userid2guidMap.erase(userid);
}

ClientAgent* GagaServer::FindClientAgent(const std::string userid) {
	auto search = m_userid2guidMap.find(userid);
	if (search != m_userid2guidMap.end()) {
		RakNet::RakNetGUID guid = search->second;
		ClientAgent *clientAgent = FindClientAgent(guid);
		if (clientAgent) {
			return clientAgent;
		}
	}
	return nullptr;
}

bool GagaServer::SendClient(const RakNet::SystemAddress& clientAddr, const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg) {
	RakNet::BitStream bitStream;
	bool ret = GagaUtil::GetInstance()->WriteBitStream(msgId, msg, bitStream);
	if (true == ret) {
		uint32_t sendRet = m_rakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, clientAddr, 0);
		if (sendRet > 0) {
			return true;
		}
	}
	return false;
}

Player* GagaServer::AcquirePlayer() {
	auto ret = m_playerPool->AcquireObject();
	ret->Reset();
	return ret;
}

void GagaServer::ReleasePlayer(Player *player) {
	m_playerPool->ReleaseObject(player);
}

uint32_t GagaServer::CreateRoom() {
	Room *room = m_roomPool->AcquireObject();
	if (room) {
		uint32_t roomNo = CalcRoomNo();
		room->set_roomNo(roomNo);
		room->set_maxPlayer(1);
		m_onlineRoomList.insert(std::make_pair(roomNo, room));
		return roomNo;
	}
	return 0;
}

void GagaServer::GetRoomList(std::vector<uint32_t>& roomList) {
	for (auto& it : m_onlineRoomList) {
		Room *room = it.second;
		if (!room->IsFull()) {
			roomList.push_back(room->get_roomNo());
		}
	}
	if (roomList.empty()) {
		uint32_t roomNo = CreateRoom();
		roomList.push_back(roomNo);
	}
}

void GagaServer::RemoveRoom(uint32_t roomNo) {
	Room *room = FindRoom(roomNo);
	if (room) {
		m_roomPool->ReleaseObject(room);
		m_onlineRoomList.erase(roomNo);
	}
}

Room* GagaServer::FindRoom(uint32_t roomNo) {
	auto search = m_onlineRoomList.find(roomNo);
	if (search != m_onlineRoomList.end()) {
		return search->second;
	}
	return nullptr;
}

uint32_t GagaServer::CalcRoomNo() {
	++m_roomCount;
	return m_roomCount;
}

void GagaServer::TestCase() {
	// ---------------------- TimerManager Test ------------------------
#if 0
	for (int i = 1; i < 65; i++) {
		std::string timerName = "timer_test";
		TimerManager::GetInstance()->AddTimeoutTimer(timerName + std::to_string(i), 64*2*i, [i](){
			gaga_printf("timer_test01: timer %d ok\n", i);
		});
	}

	TimerManager::GetInstance()->AddTimeoutTimer("5-min-timer", 64 * 60 * 5, [](){
		gaga_printf("timer_test01: 5 min timer ok\n");
	});
	TimerManager::GetInstance()->AddTimeoutTimer("20-min-timer", 64 * 60 * 20, [](){
		gaga_printf("timer_test01: 20 min timer ok\n");
	});
	
	auto ret = TimerManager::GetInstance()->AddTimeoutTimer("test-remove", 64, [](){
		gaga_printf("timer_test01: test-remove ok\n");
	});
	TimerManager::GetInstance()->RemoveTimer("test-remove");
	TimerManager::GetInstance()->AddTimeoutTimer("test-remove", 64, [](){
		gaga_printf("\ntimer_test01: test-remove2 ok\n");
	});

	TimerManager::GetInstance()->AddTickTimer("test-tick-timer01", 2*64, []() {
		gaga_printf("test-tick-timer01: tick..");
	});
	TimerManager::GetInstance()->AddTickTimer("test-tick-timer02", 64*4, []() {
		gaga_printf("test-tick-timer02: tick..");
	});
	TimerManager::GetInstance()->AddTickTimer("test-tick-timer03", 64*8, []() {
		gaga_printf("test-tick-timer03: tick..");
	});

	TimerManager::GetInstance()->AddTimeoutTimer("test-remove2", 4*64, [](){
		gaga_printf("\ntimer_test01: test-remove2 ok\n");
		TimerManager::GetInstance()->RemoveTimer("test-tick-timer01");
	});
#endif
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);
	printf("%4d/%02d/%02d %02d:%02d:%02d.%03d Week%1d\n", sys_time.wYear,
		sys_time.wMonth,
		sys_time.wDay,
		sys_time.wHour,
		sys_time.wMinute,
		sys_time.wSecond,
		sys_time.wMilliseconds,
		sys_time.wDayOfWeek);

	TimerManager::GetInstance()->AddTimeoutTimer("test-8h", 64*8*3600, [](){
		gaga_printf("\ntimer_test01: test-8h ok\n");
		SYSTEMTIME sys_time;
		GetLocalTime(&sys_time);
		printf("%4d/%02d/%02d %02d:%02d:%02d.%03d Week%1d\n", sys_time.wYear,
			sys_time.wMonth,
			sys_time.wDay,
			sys_time.wHour,
			sys_time.wMinute,
			sys_time.wSecond,
			sys_time.wMilliseconds,
			sys_time.wDayOfWeek);
	});
}


