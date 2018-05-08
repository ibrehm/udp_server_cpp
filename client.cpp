#include "client.h"

Client::Client(int num) {
	id = num;
	time(&timer);
}

int Client::GetID() {
	return(id);
}
