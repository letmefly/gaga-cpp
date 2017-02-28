#ifndef __GAGA_CLIENT_H__
#define __GAGA_CLIENT_H__

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"

class GagaClient
{
public:
	static GagaClient* GetInstance();
	GagaClient() {};
	~GagaClient() {};

	bool Start();
	void Stop();
	void Tick();
private:
	void HandlePacket(RakNet::Packet *packet);
	RakNet::RakPeerInterface *m_rakPeer;
	RakNet::SystemAddress m_serverAddr;
};


#endif