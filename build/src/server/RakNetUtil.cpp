#include "MessageIdentifiers.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"

#include "RakNetUtil.h"

bool RakNetUtil::WriteMsg(RakNet::BitStream& bitStream, RakNet::MessageID msgId, ::google::protobuf::MessageLite& msg)
{
	static char tempMsgBuffer[GAME_MSG_MAX_SIZE + 10];
	std::size_t size = (std::size_t) msg.ByteSize();
	if (size > GAME_MSG_MAX_SIZE) { return false; }
	if (msg.SerializeToArray(tempMsgBuffer, size))
	{
		bitStream.Write((RakNet::MessageID) msgId);
		bitStream.Write(tempMsgBuffer, size);
		return true;
	}
	else
		//BIRIBIT_WARN("%s unable to serialize.", msg.GetTypeName().c_str());

	return false;
}

