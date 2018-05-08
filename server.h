#ifndef __server_h__

#include<stdio.h>
#include <string>
#include <iostream>
#include<pthread.h>
#include<vector>
#include "cross_network.h"
#include "client.h"
#include "BytePacket.h"

#define __server_h__

#define PORT_NUMBER 8888
#define SERVER_CAPACITY 5
#define SERVER_TIMEOUT 30
#define BUFFER_SIZE 1472

void *ClientCapture(void *data);

//--------------------------------------------------------------------------------------
class ServerManager {

public:
	ServerManager();
	~ServerManager();
	void Startup();
	void Shutdown();
	bool OnlineStatus();
	int rtrnSocket();
	int AvailableID();
	int AddClient(int);
	void RemoveClient(int, bool);
	bool IsValid(int);
	unsigned int Connections();
	void SendMessage(std::string,int);
	int IDFromName(std::string);
	void ShowUserData(int);
	void ShowUserData(std::string);
	void CheckTimeouts();
	void Hurt(int, int);

	void handleCmd(BytePacket&, struct sockaddr_in&);

	friend void *ClientCapture(void *);
private:

	std::vector<struct sockaddr_in*> cmd_new_player(BytePacket&, struct sockaddr_in&);
	std::vector<struct sockaddr_in*> cmd_update_position(BytePacket&);
	std::vector<struct sockaddr_in*> cmd_get_surrounding_players(BytePacket&);
	std::vector<struct sockaddr_in*> cmd_send_message(BytePacket&);
	std::vector<struct sockaddr_in*> cmd_quit_player(BytePacket&);
	std::vector<struct sockaddr_in*> cmd_get_player_stats(BytePacket&);

	fd_set master;
	fd_set read_fds;

	bool online;
	int connections;

	struct sockaddr_in server;
	int rc;
	int c;
	SOCKET socket_desc;

    std::vector<Client*> clients;

	pthread_t captureClients;
	// Object that's needed for compiling in Windows.
	// Does nothing in Linux.
	WSAStart wsaObj;
};
//--------------------------------------------------------------------------------------
#endif // __server_h__
