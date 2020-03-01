#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "Logger.h"


#define SERVER "3.87.223.162"	
#define BUFLEN 512	
#define PORT 1066	


enum class CONNECTION_STATE {
	DISCONNECTED,
	PENDING,
	CONNECTED
};


class Connection {

public:
	static Connection* CreateConnection();
	CONNECTION_STATE conn_state = CONNECTION_STATE::DISCONNECTED;
	int SendInputState(char* toSend, int len);
	void AttemptConnection();
private:
	SOCKET gameSocket;
	sockaddr_in server;
	char* challenge;
	short salt;
	Connection(SOCKET _gs, sockaddr_in s) : gameSocket(_gs), server(s) {};
};
