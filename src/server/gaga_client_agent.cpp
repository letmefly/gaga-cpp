#include "gaga_client_agent.h"
#include "gaga_util.h"
#include "GameMsg.pb.h"
#include "gaga_server.h"

ClientAgent::ClientAgent() {

}

ClientAgent::~ClientAgent() {

}

void ClientAgent::HandlePacket(RakNet::Packet *packet) {
	RakNet::MessageID messageId;
	GagaUtil::GetInstance()->ParseMsgId(packet, messageId);
	if (messageId == proto::MSG_LOGIN_REQ) {
		proto::Login_req login_req;
		GagaUtil::GetInstance()->ParseMsg(packet, login_req);
		handle_login_req(login_req);
	}
	else if (messageId == proto::MSG_CREATE_ROOM_REQ) {
		proto::CreateRoom_req createRoom_req;
		GagaUtil::GetInstance()->ParseMsg(packet, createRoom_req);
		handle_createRoom_req(createRoom_req);
	}
	else if (messageId == proto::MSG_LIST_ROOM_REQ) {
		proto::ListRoom_req listRoom_req;
		GagaUtil::GetInstance()->ParseMsg(packet, listRoom_req);
		handle_listRoom_req(listRoom_req);
	}
	else if (messageId == proto::MSG_JOIN_ROOM_REQ) {
		proto::JoinRoom_req joinRoom_req;
		GagaUtil::GetInstance()->ParseMsg(packet, joinRoom_req);
		handle_joinRoom_req(joinRoom_req);
	}
	else if (messageId == proto::MSG_ENTER_GAME_OK_REQ) {
		proto::EnterGameOk_req enterGameOk_req;
		GagaUtil::GetInstance()->ParseMsg(packet, enterGameOk_req);
		handle_enterGameOk_req(enterGameOk_req);
	}
}

void ClientAgent::SendMsg(const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg) {
	GagaServer::GetInstance()->SendClient(m_addr, msgId, msg);
}

void ClientAgent::handle_login_req(const proto::Login_req& login_req) {
	const proto::User_t& userInfo = login_req.userinfo();
	gaga_printf("\nlogin userid-%s", userInfo.userid().c_str());
	gaga_printf("\nlogin nickname-%s", userInfo.nickname().c_str());
	m_userid = userInfo.userid();
	m_name = userInfo.nickname();
	GagaServer::GetInstance()->AddGUIDMap(m_userid, m_guid);
	proto::Login_ack login_ack;
	login_ack.set_error_code(proto::CODE_OK);
	GagaServer::GetInstance()->SendClient(m_addr, proto::MSG_LOGIN_ACK, login_ack);
}

void ClientAgent::handle_createRoom_req(const proto::CreateRoom_req& createRoom_req) {
	uint32_t roomNo = GagaServer::GetInstance()->CreateRoom();
	proto::CreateRoom_ack createRoom_ack;
	createRoom_ack.set_error_code(proto::CODE_OK);
	GagaServer::GetInstance()->SendClient(m_addr, proto::MSG_LOGIN_ACK, createRoom_ack);
}

void ClientAgent::handle_joinRoom_req(const proto::JoinRoom_req& joinRoom_req) {
	uint32_t roomNo = joinRoom_req.room_no();
	std::string roomPassword = joinRoom_req.room_password();
	auto playerInfo = joinRoom_req.playerinfo();
	Room *room = GagaServer::GetInstance()->FindRoom(roomNo);
	proto::ErrorCode_t errnoCode = proto::CODE_OK;
	if (room) {
		if (room->get_password() == roomPassword) {
			Player tmpPlayer;
			tmpPlayer.set_car_name(playerInfo.car_name());
			tmpPlayer.set_is_free(playerInfo.is_free());
			tmpPlayer.set_is_owned(playerInfo.is_owned());
			tmpPlayer.set_stage(playerInfo.stage());
			tmpPlayer.set_decal(playerInfo.decal());
			tmpPlayer.set_decal_color(playerInfo.decal_color());
			tmpPlayer.set_driver_name(playerInfo.driver_name());
			tmpPlayer.set_driver_type(playerInfo.driver_type());
			tmpPlayer.set_accel(playerInfo.accel());
			tmpPlayer.set_speed(playerInfo.speed());
			tmpPlayer.set_handling(playerInfo.handling());
			tmpPlayer.set_tough(playerInfo.tough());
			tmpPlayer.set_is_gold(playerInfo.is_gold());
			tmpPlayer.set_min_stage(playerInfo.min_stage());
			tmpPlayer.set_max_stage(playerInfo.max_stage());
			tmpPlayer.set_paint_color(playerInfo.paint_color());
			int ret = room->Join(m_guid, tmpPlayer);
			if (!ret) {
				m_joinRoomNo = roomNo;
			}
			else {
				errnoCode = proto::CODE_ROOM_FULL;
			}
		}
		else {
			errnoCode = proto::CODE_ROOM_PW_NOT_RIGHT;
		}
	}
	else {
		errnoCode = proto::CODE_ROOM_NOT_EXIST;
	}
	proto::JoinRoom_ack joinRoom_ack;
	joinRoom_ack.set_error_code(errnoCode);
	if (proto::CODE_OK == errnoCode) {
		joinRoom_ack.set_playerid(room->GetPlayerid(m_userid));
	}
	GagaServer::GetInstance()->SendClient(m_addr, proto::MSG_JOIN_ROOM_ACK, joinRoom_ack);
}

void ClientAgent::handle_listRoom_req(const proto::ListRoom_req& listRoom_req) {
	std::vector<uint32_t> roomList;
	GagaServer::GetInstance()->GetRoomList(roomList);

	proto::ListRoom_ack listRoom_ack;
	listRoom_ack.set_error_code(proto::CODE_OK);
	if (roomList.empty()) {
		listRoom_ack.set_error_code(proto::CODE_ROOM_FULL);
	}
	else {
		for (auto roomNo : roomList) {
			auto room = listRoom_ack.add_room_list();
			room->set_room_no(roomNo);
			room->set_desp("xixi");
		}
	}
	GagaServer::GetInstance()->SendClient(m_addr, proto::MSG_LIST_ROOM_ACK, listRoom_ack);
}

void ClientAgent::handle_enterGameOk_req(const proto::EnterGameOk_req& enterGameOk_req) {
	auto room = GagaServer::GetInstance()->FindRoom(m_joinRoomNo);
	if (room) {
		room->EnterGameOk(enterGameOk_req.playerid());
	}
}

