#ifndef __GAGA_SERVER_H__
#define __GAGA_SERVER_H__

#include <map>

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"
#include "object_pool.h"
#include "gaga_client_agent.h"
#include "gaga_room.h"
#include "gaga_player.h"

class GagaServer {
public:
	static GagaServer* GetInstance();
	GagaServer() : m_roomCount(100000) {}
	~GagaServer() {}
	bool Start();
	void Stop();
	void Tick();
	void AddGUIDMap(const std::string userid, const RakNet::RakNetGUID guid);
	void RemoveGUIDMap(const std::string userid);
	bool SendClient(const RakNet::SystemAddress& clientAddr, const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg);
	Player* AcquirePlayer();
	void ReleasePlayer(Player *player);
	uint32_t CreateRoom();
	void GetRoomList(std::vector<uint32_t>& roomList);
	Room* FindRoom(uint32_t roomNo);
	void RemoveRoom(uint32_t roomNo);
	ClientAgent* FindClientAgent(const std::string userid);
	ClientAgent* FindClientAgent(const RakNet::RakNetGUID& guid);
	ClientAgent* AddClientAgent(const RakNet::RakNetGUID& guid);
	void RemoveClientAgent(const RakNet::RakNetGUID& guid);

private:
	void HandlePacket(RakNet::Packet *packet);
	uint32_t CalcRoomNo();

	RakNet::RakPeerInterface *m_rakPeer;
	ObjectPool<ClientAgent> *m_clientAgentPool;
	ObjectPool<Player> *m_playerPool;
	ObjectPool<Room> *m_roomPool;
	std::map<RakNet::RakNetGUID, ClientAgent*> m_onlineClientList;
	std::map<uint32_t, Room*> m_onlineRoomList;
	std::map<std::string, RakNet::RakNetGUID> m_userid2guidMap;
	uint32_t m_roomCount;
	void TestCase();
};

#endif
