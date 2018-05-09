// (C) 2018 Ian Brehm
//-----------------------------------------------------------------------------------------------------------
#ifndef __client_h__
#define __client_h__

#include <stdio.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <time.h>
#include "cross_network.h"
#include "Unit.h"

class Client: Unit {

public:
	Client(int num = -1);

	int GetID();

	time_t timer;
	struct sockaddr_in client;

	friend void *ClientCapture(void *);
	friend class ServerManager;
private:
	int id;
};

#endif // __client_h__

