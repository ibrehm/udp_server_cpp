// (C) 2018 Ian Brehm
//-----------------------------------------------------------------------------------------------------------
#include "client.h"

Client::Client(int num) {
	id = num;
	time(&timer);
}

int Client::GetID() {
	return(id);
}
