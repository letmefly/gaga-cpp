#include "gaga_room.h"
#include "gaga_server.h"
#include "gaga_client_agent.h"
#include "gaga_timer.h"

int Room::Join(const RakNet::RakNetGUID& guid, Player& playerInfo) {
	if (m_playerList.size() > m_maxPlayer) {
		return -1;
	}
	Player *player = nullptr;
	ClientAgent *clientAgent = GagaServer::GetInstance()->FindClientAgent(guid);
	if (!clientAgent) { return -2; }
	std::string userid = clientAgent->get_userid();
	uint32_t playerid = GetPlayerid(userid);
	player = FindPlayer(playerid);
	if (nullptr == player) {
		player = GagaServer::GetInstance()->AcquirePlayer();
		player->Reset();
		player->set_userid(clientAgent->get_userid());
		playerid = AssignPlayerid();
		player->set_playerid(playerid);
		player->set_car_name(playerInfo.get_car_name());
		player->set_is_free(playerInfo.get_is_free());
		player->set_is_owned(playerInfo.get_is_owned());
		player->set_stage(playerInfo.get_stage());
		player->set_decal(playerInfo.get_decal());
		player->set_decal_color(playerInfo.get_decal_color());
		player->set_driver_name(playerInfo.get_driver_name());
		player->set_driver_type(playerInfo.get_driver_type());
		player->set_accel(playerInfo.get_accel());
		player->set_speed(playerInfo.get_speed());
		player->set_handling(playerInfo.get_handling());
		player->set_tough(playerInfo.get_tough());
		player->set_is_gold(playerInfo.get_is_gold());
		player->set_min_stage(playerInfo.get_min_stage());
		player->set_max_stage(playerInfo.get_max_stage());
		player->set_paint_color(playerInfo.get_paint_color());
		m_playerList.insert(std::make_pair(playerid, player));
	}
	// binding client agent to player with guid
	if (player) {
		player->set_userGuid(guid);
	}
	else {
		return -3;
	}

	PlayerListNtf();

	if (m_playerList.size() == m_maxPlayer) {
		TimerManager::GetInstance()->AddTimeoutTimer("start-game", 64, [this]() {
			this->EnterGameNtf();
		});
	}

	return 0;
}

uint32_t Room::GetPlayerid(const std::string userid) {
	proto::PlayerList_ntf playerList_ntf;
	for (auto it : m_playerList) {
		Player *player = it.second;
		if (player && player->get_userid() == userid) {
			return player->get_playerid();
		}
	}
	return 0;
}

uint32_t Room::AssignPlayerid() {
	return m_playerList.size() + 1;
}

int Room::GetReady(const std::string& userid) {
	auto playerid = GetPlayerid(userid);
	Player *player = FindPlayer(playerid);
	if (player) {
		player->GetReady();
		return 0;
	}
	return -1;
}

void Room::Leave(const RakNet::RakNetGUID& guid) {

}

int Room::StartGame() {
	Reset();
	m_nextLogicFrame = new LogicFrame();
	m_nextLogicFrame->frameNo = m_frameNo;
	int ret = TimerManager::GetInstance()->AddTickTimer("ROOM-" + std::to_string(m_roomNo), 6, [this]() {this->Tick(); });
	if (ret != 0) {
		return -1;
	}
	return 0;
}

void Room::StopGame() {
	TimerManager::GetInstance()->RemoveTimer("ROOM-" + std::to_string(m_roomNo));
}

void Room::Tick() {
	++m_frameNo;
	if (m_frameNo > 64 * 60 * 15) return;
	LogicFrameNtf();
	m_logicFrameList.push_back(m_nextLogicFrame);
	m_nextLogicFrame = new LogicFrame();
	m_nextLogicFrame->frameNo = m_frameNo;

	//gaga_printf("Room Tick ...");
}

void Room::Reset() {
	m_frameNo = 0;
	for (auto it : m_playerList) {
		Player *player = it.second;
		player->Reset();
	}
}

void Room::SendAllPlayer(const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg) {
	for (auto it : m_playerList) {
		ClientAgent *client = FindClientAgent(it.second->get_playerid());
		if (client) {
			client->SendMsg(msgId, msg);
		}
	}
}

void Room::SendPlayer(const uint32_t playerid, RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg) {
	ClientAgent *client = FindClientAgent(playerid);
	if (client) {
		client->SendMsg(msgId, msg);
	}
}

Player* Room::FindPlayer(const uint32_t playerid) {
	auto it = m_playerList.find(playerid);
	if (it != m_playerList.end()) {
		return it->second;
	}
	return nullptr;
}

ClientAgent* Room::FindClientAgent(const uint32_t playerid) {
	Player *player = FindPlayer(playerid);
	if (player) {
		return GagaServer::GetInstance()->FindClientAgent(player->get_userGuid());
	}
	return nullptr;
}

bool Room::IsFull() {
	return m_playerList.size() >= m_maxPlayer;
}

void Room::PlayerListNtf() {
	proto::PlayerList_ntf playerList_ntf;
	for (auto it : m_playerList) {
		Player *player = it.second;
		auto player_t = playerList_ntf.add_player_list();
		player_t->set_playerid(player->get_playerid());
		player_t->set_car_name(player->get_car_name());
		player_t->set_is_free(player->get_is_free());
		player_t->set_is_owned(player->get_is_owned());
		player_t->set_stage(player->get_stage());
		player_t->set_decal(player->get_decal());
		player_t->set_decal_color(player->get_decal_color());
		player_t->set_driver_name(player->get_driver_name());
		player_t->set_driver_type(player->get_driver_type());
		player_t->set_accel(player->get_accel());
		player_t->set_speed(player->get_speed());
		player_t->set_handling(player->get_handling());
		player_t->set_tough(player->get_tough());
		player_t->set_is_gold(player->get_is_gold());
		player_t->set_min_stage(player->get_min_stage());
		player_t->set_max_stage(player->get_max_stage());
		player_t->set_paint_color(player->get_paint_color());
	}
	SendAllPlayer(proto::MSG_PLAYER_LIST_NTF, playerList_ntf);
}

void Room::StartRaceNtf() {
	proto::StartRace_ntf startRace_ntf;
	SendAllPlayer(proto::MSG_START_RACE_NTF, startRace_ntf);
}

void Room::EnterGameNtf() {
	proto::EnterGame_ntf enterGame_ntf;
	for (auto it : m_playerList) {
		Player *player = it.second;
		auto player_t = enterGame_ntf.add_player_list();
		player_t->set_playerid(player->get_playerid());
	}
	SendAllPlayer(proto::MSG_ENTER_GAME_NTF, enterGame_ntf);
}

void Room::EnterGameOk(uint32_t playerid) {
	auto player = FindPlayer(playerid);
	if (player) {
		player->set_state(kPlayerEnterRoomOk);
	}
	int enterGameOkNum = 0;
	for (auto it : m_playerList) {
		Player *player = it.second;
		if (player->get_state() == kPlayerEnterRoomOk) {
			enterGameOkNum++;
		}
	}
	if (enterGameOkNum == m_maxPlayer) {
		StartRaceNtf();
		TimerManager::GetInstance()->AddTimeoutTimer("start-game", 64, [this]() {
			this->StartGame();
		});
	}
}

void Room::SubmitCommand(const GameCommand &command) {
	GameCommand *newCommand = new GameCommand();
	newCommand->commandType = command.commandType;
	newCommand->playerid = command.playerid;
	newCommand->intVar1 = command.intVar1;
	newCommand->intVar2 = command.intVar2;
	newCommand->intVar3 = command.intVar3;
	newCommand->strVar1 = command.strVar1;
	newCommand->strVar2 = command.strVar2;
	newCommand->strVar3 = command.strVar3;
	m_nextLogicFrame->commandList.push_back(newCommand);
}

void Room::LogicFrameNtf() {
	proto::LogicFrame_ntf logicFrame_ntf;
	logicFrame_ntf.set_frame_no(m_nextLogicFrame->frameNo);
	for (auto command : m_nextLogicFrame->commandList) {
		auto cmd = logicFrame_ntf.add_command_list();
		cmd->set_command_type(command->commandType);
		cmd->set_playerid(command->playerid);
		cmd->set_intvar1(command->intVar1);
		cmd->set_intvar2(command->intVar2);
		cmd->set_intvar3(command->intVar3);
		cmd->set_strvar1(command->strVar1);
		cmd->set_strvar2(command->strVar2);
		cmd->set_strvar3(command->strVar3);
	}
	SendAllPlayer(proto::MSG_LOGIC_FRAME_NTF, logicFrame_ntf);
}

void Room::Distroy() {
	for (auto logicFrame : m_logicFrameList) {
		for (auto command : logicFrame->commandList) {
			delete command;
		}
		logicFrame->commandList.clear();
		delete logicFrame;
	}
	for (auto command : m_nextLogicFrame->commandList) {
		delete command;
	}
	delete m_nextLogicFrame;
}

