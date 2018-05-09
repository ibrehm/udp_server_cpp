// (C) 2018 Ian Brehm
//-----------------------------------------------------------------------------------------------------------
#include "server.h"
#include "utils.h"
//--------------------------------------------------------------------------------------
// Start up the server, initialize what we need
ServerManager::ServerManager() {
	connections = 0;
	Startup();
}
//--------------------------------------------------------------------------------------
// Makes sure we clean up
ServerManager::~ServerManager() {
	Shutdown();
}
//--------------------------------------------------------------------------------------
// Starts the server up
void ServerManager::Startup() {
	if(online == false) {
		online = true;

		// Create a reserve of clients
		clients.assign(SERVER_CAPACITY, NULL);

		// Obtain the socket
		socket_desc = socket(AF_INET , SOCK_DGRAM , 0);

		// Set various settings
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons( PORT_NUMBER );

		// Try to bind the socket.
		if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
			std::cout << "class bind failed\n";
		}

		c = sizeof(struct sockaddr_in);

		// This is used to find out when there's data to be received.
		// It is used in conjunction with select() in the thread created below
		FD_ZERO(&master);
		FD_ZERO(&read_fds);
		FD_SET(socket_desc, &master);

		// Creates a new thread within the program to run on. It listens to incoming
		// connections from other clients.
		rc = pthread_create(&captureClients, NULL, ClientCapture, (void*)this);
		if(rc){
			std::cout << "ERROR; return code from pthread_create() is " << rc << "\n";
		}
	}
}
//--------------------------------------------------------------------------------------
// Removes a client from the manager
void ServerManager::RemoveClient(int num, bool notify = false) {
	if((num < SERVER_CAPACITY) && (num >= 0)) {
		if(clients[num] != NULL) {
			delete clients[num];
			clients[num] = NULL;
			connections--;

			if(notify == true) {
				BytePacket packet;
				packet.SetIntent(CMD_QUIT_PLAYER, num);
				for(unsigned int i = 0; i < clients.size(); i++) {
					if(clients[i] != NULL) {
						sendto(socket_desc, packet.rtrnData(), packet.Size(), 0, (struct sockaddr *)&clients[i]->client, c);
					}
				}
			}
		}
	}
}
//--------------------------------------------------------------------------------------
// Shuts the server down
void ServerManager::Shutdown() {
	if(online == true) {
		online = false;

		// Wait for thread to end
		void *ret;
		pthread_join(captureClients, &ret);

		// Go through all the clients and remove ones that aren't null
		for(unsigned int i = 0; i < clients.size(); i++) {
			if(clients[i] != NULL) {
				RemoveClient(i);
			}
		}
		// Clear the server socket
    	FD_CLR(socket_desc, &master);
		// close the socket
		closesocket(socket_desc);
    }
}
//--------------------------------------------------------------------------------------
// Returns the online status of the server
bool ServerManager::OnlineStatus() {
	return(online);
}
//--------------------------------------------------------------------------------------
// Returns the socket being used by the server
int ServerManager::rtrnSocket() {
	return(socket_desc);
}
//--------------------------------------------------------------------------------------
// Returns the first available ID within the clients
int ServerManager::AvailableID() {
	for(int i = 0; i < SERVER_CAPACITY; i++) {
		if(clients[i] == NULL) {
			return(i);
		}
	}
	return(-1);
}
//--------------------------------------------------------------------------------------
// Adds a new client to the manager. The desired client ID can be specified, and if
// available, will be given as requested. Returns -1 if it could not create a new client.
int ServerManager::AddClient(int num = -1) {

	if( (num >= 0) && (num < SERVER_CAPACITY) ) {
		if(clients[num] == NULL) {
			clients[num] = new Client(num);
			connections++;
			return(num);
		}
	} else {
		int new_id = AvailableID();
		if(new_id != -1) {
			clients[new_id] = new Client(num);
			connections++;
			return(new_id);
		}
	}
	return(-1);
}
//--------------------------------------------------------------------------------------
// Checks to see if a client ID is being used
bool ServerManager::IsValid(int num) {
	if((num < SERVER_CAPACITY) && (num >= 0)) {
		if(clients[num] != NULL) {
			return(true);
		}
	}
	return(false);
}
//--------------------------------------------------------------------------------------
// Returns the amount of connections received thus far
unsigned int ServerManager::Connections() {
	return(connections);
}
//--------------------------------------------------------------------------------------
// Send a message to a specified client. It adds a null terminal character to the end
// of the string so that the string on the client side ends at the proper place.
void ServerManager::SendMessage(std::string message, int id) {
	BytePacket send;

	if(id == -1) {
		send.SetIntent(CMD_SEND_MESSAGE, -1);
		send.Add<std::string>(message);
		for(unsigned int i = 0; i < clients.size(); i++) {
			sendto(rtrnSocket(), send.rtrnData(), send.Size(), 0, (struct sockaddr *)&clients[i]->client, c);
		}
	} else {
		if(clients[id] != NULL) {
			send.SetIntent(CMD_SEND_MESSAGE, -1);
			send.Add<std::string>(message);
			sendto(rtrnSocket(), send.rtrnData(), send.Size(), 0, (struct sockaddr *)&clients[id]->client, c);
		}
	}
}
//--------------------------------------------------------------------------------------
// Returns the user ID based on name
int ServerManager::IDFromName(std::string name) {
	for(unsigned int i = 0; i < clients.size(); i++) {
		if(clients[i] != NULL) {
			if(clients[i]->name == name) {
				return(i);
			}
		}
	}
	return(-1);
}
//--------------------------------------------------------------------------------------
// Shows user data, such as X/Y positions in the game world
void ServerManager::ShowUserData(int id) {
	if(clients[id] != NULL) {

		std::cout << "\tID:\t" << id << "\n";
		std::cout << "\tName:\t" << clients[id]->name << "\n";
		std::cout << "\tX:\t" << patch::to_string(clients[id]->x) << "\n";
		std::cout << "\tY:\t" << patch::to_string(clients[id]->y) << "\n";

	}
}
//--------------------------------------------------------------------------------------
// Shows user data, such as X/Y positions in the game world
void ServerManager::ShowUserData(std::string name) {
	for(unsigned int i = 0; i < clients.size(); i++) {
		if(clients[i] != NULL) {
			if(clients[i]->name == name) {
				ShowUserData(i);
				break;
			}
		}
	}
}
//--------------------------------------------------------------------------------------
// Goes through all the clients and compares the last time they were active. If it's
// greater than SERVER_TIMEOUT, we assume the client has timed out and remove them.
void ServerManager::CheckTimeouts() {
	time_t now;
	time(&now);
	double difference = 0;
	for(int i = 0; i < SERVER_CAPACITY; i++) {
		if(clients[i] != NULL) {
			difference = difftime(now, clients[i]->timer);
			if(difference >= SERVER_TIMEOUT) {
				RemoveClient(i, true);
				std::cout << "Client #" << i << " timed out.\n";
			}
		}
	}
}
//--------------------------------------------------------------------------------------
// Creates a new user on the server if there's an available slot on the server
std::vector<struct sockaddr_in*> ServerManager::cmd_new_player(BytePacket &packet, struct sockaddr_in &remote_addr) {

	std::vector<struct sockaddr_in*> rtrn;

	int new_id = AvailableID();

	// If there's an available ID, we do this...
	if(new_id != -1) {
		AddClient(new_id);

		clients[new_id]->x = packet.Get<float>();
		clients[new_id]->y = packet.Get<float>();

		std::cout << "Client #" << new_id << " Connected\n";
		// This stores the remote address so we can send unrequested packets
		// later on if we need to.
		clients[new_id]->client = remote_addr;

	}

	// Set a new intent, confirming new ID
	packet.SetIntent(CMD_SET_NEW_ID, new_id);
	rtrn.push_back(&remote_addr);

	return(rtrn);
}
//--------------------------------------------------------------------------------------
// Updates a user's position on the server
std::vector<struct sockaddr_in*> ServerManager::cmd_update_position(BytePacket &packet) {
	std::vector<struct sockaddr_in*> rtrn;
	int id = packet.GetHeader<int>(ID_START);
	// Pull out the data sent.
	//std::cout << "ID " << id << " is updating.\n";

	float x = packet.Get<float>();
	float y = packet.Get<float>();

	clients[id]->x = x;
	clients[id]->y = y;

	return(rtrn);
}
//--------------------------------------------------------------------------------------
// Obtains potential players around the player who sent the request
std::vector<struct sockaddr_in*> ServerManager::cmd_get_surrounding_players(BytePacket &packet) {
	std::vector<struct sockaddr_in*> rtrn;
	//std::vector<Client*> players;
	int id = packet.GetHeader<int>(ID_START);
	int total_players = 0;

	packet.SetIntent(CMD_GET_SURROUNDING_PLAYERS, id);
	// Add total players to the packet so we can change it later
	packet.Add<int>(total_players);

	int otherX = 0;
	int otherY = 0;
	int playerX = clients[id]->x;
	int playerY = clients[id]->y;

	/*
	unsigned int i = 0;
	while(i < clients.size()) {
		if((i != (unsigned int)id) && (clients[i] != NULL)) {
			otherX = clients[i]->x;
			otherY = clients[i]->y;
			float distance = Distance(playerX, playerY, otherX, otherY);
			if(distance < 35.0f) {
				packet.Add<int>(clients[i]->id);
				packet.Add<float>(clients[i]->x);
				packet.Add<float>(clients[i]->y);
				packet.Add<std::string>(clients[i]->name);
				total_players++;
			}
		}
		i++;
	}
	*/

	for(unsigned int i = 0; i < clients.size(); i++) {
		if((i != (unsigned int)id) && (clients[i] != NULL)) {
			otherX = clients[i]->x;
			otherY = clients[i]->y;
			float distance = Distance(playerX, playerY, otherX, otherY);
			if(distance < 35.0f) {
				packet.Add<int>(clients[i]->id);
				packet.Add<float>(clients[i]->x);
				packet.Add<float>(clients[i]->y);
				packet.Add<std::string>(clients[i]->name);
				total_players++;
			}
		}
	}
	// update amount of players being sent to the user
	packet.Update<int>(0, total_players);

	rtrn.push_back(&clients[id]->client);

	return(rtrn);
}
//--------------------------------------------------------------------------------------
// Send a message to available clients
std::vector<struct sockaddr_in*> ServerManager::cmd_send_message(BytePacket &packet) {
	std::vector<struct sockaddr_in*> rtrn;
	int id = packet.GetHeader<int>(ID_START);

	clients[id]->name = packet.Get<std::string>();

	for(unsigned int i = 0; i < clients.size(); i++) {
		if((i != (unsigned int)id) && (clients[i] != NULL)) {
			rtrn.push_back(&clients[i]->client);
		}
	}
	return(rtrn);
}
//--------------------------------------------------------------------------------------
// When a user quits, this command is sent so the server can free the slot/memory
std::vector<struct sockaddr_in*> ServerManager::cmd_quit_player(BytePacket &packet) {
	std::vector<struct sockaddr_in*> rtrn;
	int id = packet.GetHeader<int>(ID_START);
	// Display it in the console and remove them
	std::cout << "Client #" << id << " Disconnected\n";
	RemoveClient(id);

	packet.SetIntent(CMD_QUIT_PLAYER, id);
	for(unsigned int i = 0; i < clients.size(); i++) {
		if(clients[i] != NULL) {
			rtrn.push_back(&clients[i]->client);
		}
	}
	return(rtrn);
}
//--------------------------------------------------------------------------------------
// Obtains various game stats for a specified user
std::vector<struct sockaddr_in*> ServerManager::cmd_get_player_stats(BytePacket &packet) {
	std::vector<struct sockaddr_in*> rtrn;
	int id = packet.GetHeader<int>(ID_START);

	rtrn.push_back(&clients[id]->client);
	packet.SetIntent(CMD_GET_PLAYER_STATS, id);

	packet.Add<std::string>(clients[id]->name);
	packet.Add<int>(clients[id]->level);
	packet.Add<int>(clients[id]->exp);

	for(unsigned int i = 0; i < clients[id]->m_stats.size(); i++) {
		packet.Add<int>(clients[id]->c_stats[i]);
		packet.Add<int>(clients[id]->m_stats[i]);
	}

	return(rtrn);
}
//--------------------------------------------------------------------------------------
// Alters the HP of a user
void ServerManager::Hurt(int id, int amount) {
	if(IsValid(id)) {
		clients[id]->c_stats[STAT_hp] -= amount;
	}
}
//--------------------------------------------------------------------------------------
// Determines the command within the packet received by the server
void ServerManager::handleCmd(BytePacket &packet, struct sockaddr_in &remote_addr) {
	std::vector<struct sockaddr_in*> reply_list;
	int cmd = packet.GetHeader<int>(CMD_START);
	int id = packet.GetHeader<int>(ID_START);
	if(IsValid(id)) {
		// Set the time we received this command
		time(&clients[id]->timer);
		switch(cmd) {
			// If it's just sending positional data, we do this
			case CMD_SHOW_POSITION: {
				reply_list = cmd_update_position(packet);
				break;
			}
			case CMD_GET_SURROUNDING_PLAYERS: {
				reply_list = cmd_get_surrounding_players(packet);
				break;
			}
			case CMD_SEND_MESSAGE: {
				reply_list = cmd_send_message(packet);
				break;
			}
			// If the player is quitting, we do this.
			case CMD_QUIT_PLAYER: {
				reply_list = cmd_quit_player(packet);
				break;
			}

			case CMD_GET_PLAYER_STATS: {
				reply_list = cmd_get_player_stats(packet);
				break;
			}
		}
	} else if(cmd == CMD_NEW_PLAYER) {
		reply_list = cmd_new_player(packet, remote_addr);
	}

	// Send the packet/response to all clients in the reply list
	for(unsigned int i = 0; i < reply_list.size(); i++) {
		sendto(socket_desc, packet.rtrnData(), packet.Size(), 0, (struct sockaddr *)reply_list[i], c);
	}
}
//--------------------------------------------------------------------------------------
// Function runs on a seperate thread. It listens to incoming connections and adds them
// to a vector so that the server can broadcast messages to multiple clients.
void *ClientCapture(void *data) {

	ServerManager *server = (ServerManager*)data;

	BytePacket packet;
	int fd_max = server->rtrnSocket();
	//int recv_length = 0;
	char buffer[BUFFER_SIZE];

	struct sockaddr_in remote_addr;

	for(int j = 0; j < BUFFER_SIZE; j++) {
		buffer[j] = '\0';
	}

    // Holder for timeout information
    struct timeval tv;

    std::cout << "Waiting for incoming connections...\n";
    // Keep looping until the server is "offline"
    while(server->OnlineStatus()) {
		server->read_fds = server->master; // copy it
		tv = {0, 500000};
		// If something goes wrong, we stop and shutdown. Select returns -1 on error,
		// 0 when it times out (From using tv above), or greater when data is ready.
		if (select(fd_max+1, &server->read_fds, NULL, NULL, &tv) == -1) {
        	std::cout << "Something happened, shutting down...\n";
            //perror("select");
            server->Shutdown();
        } else {
        	// Make sure we got something we want to read
			if (FD_ISSET(server->rtrnSocket(), &server->read_fds)) {
				// Receive the data
				int length = recvfrom(server->rtrnSocket(), buffer, BUFFER_SIZE, (struct sockaddr *)&remote_addr, (void*)&server->c);

				// Add the data to the packet for easy reading
				packet.Append(buffer, length);

				// Have the server handle the command/packet
				server->handleCmd(packet, remote_addr);

				// flush the buffer
				for(int i = 0; i < BUFFER_SIZE; i++) {
					buffer[i] = '\0';
				}
			}
        }
        // Checking for timeouts
        server->CheckTimeouts();
    }
    // quit the thread
	pthread_exit(NULL);
	// Return is added just to make compiler stop complaining there's no return
	return((void*)NULL);
}
