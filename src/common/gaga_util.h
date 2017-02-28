#ifndef __RAKNET_UTIL_H__
#define __RAKNET_UTIL_H__

#include "RakPeerInterface.h"
#include <RakNetStatistics.h>
#include <RakNetTypes.h>
#include <BitStream.h>
#include <RakSleep.h>
#include <PacketLogger.h>
#include <stdio.h>
#include <string>
#include "GameMsg.pb.h"

#define gaga_printf printf
#define gaga_log printf

#define PROPERTY_DEF(type, name)\
private:\
	type m_##name; \
public:\
	inline void set_##name(type v) {\
	m_##name = v; \
	}\
	inline type get_##name() {\
	return m_##name; \
	}\

#define GAME_MSG_MAX_SIZE 1024

class GagaUtil
{
public:
	static GagaUtil* GetInstance();
	GagaUtil() {}
	~GagaUtil() {}
	bool SendMsg(RakNet::RakPeerInterface *rakNetPeer, const RakNet::SystemAddress& destAddr, const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg);
	bool ParseMsgId(const RakNet::Packet *packet, RakNet::MessageID& msgId);
	bool ParseMsg(const RakNet::Packet *packet, ::google::protobuf::MessageLite& msg);
	bool WriteBitStream(const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg, RakNet::BitStream& bitStream);
private:
	char m_tempBuffer[GAME_MSG_MAX_SIZE];
};

#endif
