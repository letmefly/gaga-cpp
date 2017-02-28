#ifndef __GAGA_ROOM_H__
#define __GAGA_ROOM_H__

#include <string>
#include <map>
#include "RakNetTypes.h"
#include "gaga_util.h"
#include "gaga_player.h"
#include "gaga_client_agent.h"

struct GameCommand {
	uint32_t commandType;
	uint32_t playerid;
	int32_t intVar1;
	int32_t intVar2;
	int32_t intVar3;
	std::string strVar1;
	std::string strVar2;
	std::string strVar3;
};

struct LogicFrame {
	uint32_t frameNo;
	std::vector<GameCommand*> commandList;
};

class Room {
public:
	Room() {}
	~Room() {}
	int Join(const RakNet::RakNetGUID& guid, Player& playerInfo);
	bool IsFull();
	int GetReady(const std::string& userid);
	void Leave(const RakNet::RakNetGUID& guid);
	void EnterGameOk(uint32_t playerid);
	void SubmitCommand(const GameCommand &command);
	int StartGame();
	void StopGame();
	uint32_t GetPlayerid(const std::string userid);
	void Tick();

	void Distroy();

protected:
	void Reset();
	void PlayerListNtf();
	void EnterGameNtf();
	void StartRaceNtf();
	void LogicFrameNtf();
	uint32_t AssignPlayerid();
	void SendAllPlayer(const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg);
	void SendPlayer(const uint32_t playerid, RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg);
	
	Player* FindPlayer(const uint32_t playerid);
	ClientAgent* FindClientAgent(const uint32_t playerid);
	std::map<uint32_t, Player*> m_playerList;
	uint64_t m_frameNo;
	LogicFrame *m_nextLogicFrame;
	std::vector<LogicFrame*> m_logicFrameList;
	// ------------------- property defination ------------------- 
	PROPERTY_DEF(uint32_t, maxPlayer);
	PROPERTY_DEF(uint32_t, roomNo);
	PROPERTY_DEF(std::string, password);
};

#endif

