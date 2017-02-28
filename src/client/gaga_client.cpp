#include "gaga_client.h"
#include "gaga_util.h"
#include "GameMsg.pb.h"

static GagaClient* g_GagaClientInstance = nullptr;
GagaClient* GagaClient::GetInstance() {
	if (nullptr == g_GagaClientInstance) {
		g_GagaClientInstance = new GagaClient();
	}
	return g_GagaClientInstance;
}

bool GagaClient::Start() {
	RakNet::RakNetStatistics *rss;
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	m_rakPeer = RakNet::RakPeerInterface::GetInstance();
	char *ip = "127.0.0.1";
	char *serverPort = "8989";
	char *clientPort = "7979";
	m_rakPeer->AllowConnectionResponseIPMigration(false);
	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger, and false for low priority threads
	RakNet::SocketDescriptor socketDescriptor(0, 0);
	socketDescriptor.socketFamily = AF_INET;
	m_rakPeer->Startup(8, &socketDescriptor, 1);
	m_rakPeer->SetOccasionalPing(true);
	RakNet::ConnectionAttemptResult ret = m_rakPeer->Connect(ip, atoi(serverPort), "fuck", (int)strlen("fuck"));
	RakAssert(ret == RakNet::CONNECTION_ATTEMPT_STARTED);

	return true;
}

void GagaClient::Stop() {
	RakNet::RakPeerInterface::DestroyInstance(m_rakPeer);
}

void GagaClient::Tick() {
	RakNet::Packet *packet;
	for (packet = m_rakPeer->Receive(); packet; m_rakPeer->DeallocatePacket(packet), packet = m_rakPeer->Receive()) {
		HandlePacket(packet);
	}
}

void GagaClient::HandlePacket(RakNet::Packet *packet) {
	RakNet::MessageID messageId = (unsigned char)(packet->data[0]);
	if (messageId == ID_NEW_INCOMING_CONNECTION) {
	}
	else if (messageId == ID_DETECT_LOST_CONNECTIONS) {

	}
	else if (messageId == ID_CONNECTION_REQUEST_ACCEPTED) {
		m_serverAddr = packet->systemAddress;
		proto::Login_req login_req;
		proto::User_t* userInfo = login_req.mutable_userinfo();
		userInfo->set_userid("a123456");
		userInfo->set_nickname("chris.li");
		GagaUtil::GetInstance()->SendMsg(m_rakPeer, m_serverAddr, proto::MSG_LOGIN_REQ, login_req);
	}
	else if (messageId >= proto::MSG_RESERVE) {
		if (messageId == proto::MSG_LOGIN_ACK) {
			printf("login ack");
		}
	}
}

