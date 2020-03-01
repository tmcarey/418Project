#include "Connection.h"


Connection* Connection::CreateConnection() {
	WSADATA wsaData;

	srand(time(NULL));
	SOCKET clientSocket;
	sockaddr_in server;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup Failed.");
		return NULL;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	if (inet_pton(AF_INET, SERVER, &server.sin_addr.s_addr) != 1) {
		LogError("Failed to resolve server address.");
	}

	clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clientSocket == INVALID_SOCKET) {
		LogError("Failed to create socket.");
	}
	Connection* out = new Connection(clientSocket, server);
	out->challenge = new char[17];
	return out;
}

int Connection::SendInputState(char* toSend, int len) {
	return sendto(gameSocket, toSend, len, 0, (sockaddr*)&server, sizeof(server));
}

void Connection::AttemptConnection()
{
	int serverLen = sizeof(server);
	char inBuf[1024];
	if (recvfrom(gameSocket, inBuf, 24, 0, (sockaddr*)&server, &serverLen) > 0) {
		if (inBuf[0] == 'c') {
			memset(challenge, '\0', 17);
			for (int i = 0; i < 16; i++) {
				challenge[i] = inBuf[i + 5];
			}
			short clientSalt = (((short)inBuf[1]) << 8) | (0x00ff & inBuf[2]);
			short serverSalt = (((short)inBuf[3]) << 8) | (0x00ff & inBuf[4]);
			salt = clientSalt ^ serverSalt;
			conn_state = CONNECTION_STATE::PENDING;
		}
		else if (conn_state == CONNECTION_STATE::PENDING && inBuf[0] == 'r') {
			short inSalt = (((short)inBuf[1]) << 8) | (0x00ff & inBuf[2]);
			if (inSalt == salt) {
				Log("Successfully Connected to Server!");
				conn_state = CONNECTION_STATE::CONNECTED;
			}
		}
	}
	if (conn_state == CONNECTION_STATE::DISCONNECTED) {
		short clientSalt = rand();
		char outBuf[1024];
		memset(outBuf, 0xff, 1024);
		outBuf[0] = 'c';
		outBuf[1] = (char)(clientSalt >> 8);
		outBuf[2] = (char)(clientSalt & 0x00ff);
		sendto(gameSocket, outBuf, 1024, 0, (sockaddr*)&server, sizeof(server));
	}
	else if (conn_state == CONNECTION_STATE::PENDING) {
		char outBuf[1024];
		memset(outBuf, 0xff, 1024);
		outBuf[0] = 'q';
		for (int i = 0; i < 16; i++) {
			outBuf[i + 3] = challenge[i];
		}
		outBuf[1] = (char)(salt >> 8);
		outBuf[2] = (char)(salt & 0x00ff);
		sendto(gameSocket, outBuf, 1024, 0, (sockaddr*)&server, sizeof(server));
	}

}
