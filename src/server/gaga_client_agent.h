#ifndef __CLIENT_AGENT_H__
#define __CLIENT_AGENT_H__
#include <string>
#include "RakPeerInterface.h"
#include "GameMsg.pb.h"
#include "gaga_util.h"

class ClientAgent {
public:
	ClientAgent();
	~ClientAgent();
	void HandlePacket(RakNet::Packet *packet);
	void SendMsg(const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg);

	// message handler
	void handle_login_req(const proto::Login_req& login_req);
	void handle_createRoom_req(const proto::CreateRoom_req& createRoom_req);
	void handle_joinRoom_req(const proto::JoinRoom_req& joinRoom_req);
	void handle_listRoom_req(const proto::ListRoom_req& listRoom_req);
	void handle_enterGameOk_req(const proto::EnterGameOk_req& enterGameOk_req);

	// ------------------- property defination ------------------- 
	PROPERTY_DEF(std::string, userid);
	PROPERTY_DEF(std::string, name);
	PROPERTY_DEF(RakNet::SystemAddress, addr);
	PROPERTY_DEF(RakNet::RakNetGUID, guid);
	PROPERTY_DEF(uint32_t, joinRoomNo);
	// -------------------------- end ---------------------------- 
	/*
	// ------------------- getter and setter ------------------- 
	std::string get_userid() { return m_userid; }
	void set_userid(std::string userid) { m_userid = userid; }
	std::string get_name() { return m_name; }
	void set_name(std::string name) { m_name = name; }
	void set_addr(RakNet::SystemAddress addr) { m_addr = addr; }
	RakNet::SystemAddress get_addr() { return m_addr; }
	void set_guid(RakNet::RakNetGUID guid) { m_guid = guid; }
	RakNet::RakNetGUID get_guid() { return m_guid; }
private:
	std::string m_userid;
	std::string m_name;
	RakNet::SystemAddress m_addr;
	RakNet::RakNetGUID m_guid;
	*/
};

#endif
