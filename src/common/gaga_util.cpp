#include "gaga_util.h"

static GagaUtil *g_RakNetInstance = nullptr;

GagaUtil* GagaUtil::GetInstance() {
	if (nullptr == g_RakNetInstance) {
		g_RakNetInstance = new GagaUtil();
	}
	return g_RakNetInstance;
}

bool GagaUtil::SendMsg(RakNet::RakPeerInterface *rakNetPeer, const RakNet::SystemAddress& destAddr, const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg) {
	RakNet::BitStream bitStream;
	bool ret = WriteBitStream(msgId, msg, bitStream);
	if (ret == true) {
		uint32_t sendRet = rakNetPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, destAddr, 0);
		if (sendRet > 0) {
			return true;
		}
	}
	return false;
}

bool GagaUtil::WriteBitStream(const RakNet::MessageID msgId, const ::google::protobuf::MessageLite& msg, RakNet::BitStream& bitStream) {
	int size = (std::size_t) msg.ByteSize();
	if (msg.SerializeToArray(m_tempBuffer, GAME_MSG_MAX_SIZE))
	{
		bitStream.Write((RakNet::MessageID) msgId);
		bitStream.Write(m_tempBuffer, size);
		return true;
	}
	else
		//BIRIBIT_WARN("%s unable to serialize.", msg.GetTypeName().c_str());
	return false;
}

bool GagaUtil::ParseMsg(const RakNet::Packet *packet, ::google::protobuf::MessageLite& msg) {
	return msg.ParseFromArray(packet->data + 1, packet->length - 1);
}

bool GagaUtil::ParseMsgId(const RakNet::Packet *packet, RakNet::MessageID& msgId) {
	msgId = (unsigned char)(packet->data[0]);
	return true;
}

