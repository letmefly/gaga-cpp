#include <stdio.h>
#include <codecvt>
#include <RakPeerInterface.h>
#include "gaga_util.h"
#include "gaga_client.h"

int main(int argc, char** argv)
{
	GagaClient::GetInstance()->Start();
	while (1) {
		GagaClient::GetInstance()->Tick();
	}
	GagaClient::GetInstance()->Stop();
	return 0;
}

