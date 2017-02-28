#include <stdio.h>
#include <future>
#include <RakPeerInterface.h>
#include "gaga_server.h"

int main(int argc, char** argv)
{
	GagaServer::GetInstance()->Start();
	while (1) {
		GagaServer::GetInstance()->Tick();
		//RakSleep(30);
	}
	GagaServer::GetInstance()->Stop();
	return 0;
}
