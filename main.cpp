// (C) 2018 Ian Brehm
//-----------------------------------------------------------------------------------------------------------
#include <string>
#include <iostream>
#include "server.h"
#include "utils.h"

int main(int argc , char *argv[])
{
	// New server manager
	ServerManager server;

	// As long as the user on the server never types "quit", it will keep sending messages to the clients
	std::string message = "";
	while(message != "/quit") {
		getline(std::cin, message);

		//server.SendMessage(message);

		if(message[0] == '/') {

			std::vector<std::string> split_mes = splitStr(message, ' ', '"');

			if(split_mes[0] == "/shutdown") {
				std::cout << "Server shutting down...\n";
				server.Shutdown();
				std::cout << "Server shutdown successful.\n";
			} else if(split_mes[0] == "/startup") {
				std::cout << "Server starting up...\n";
				server.Startup();
			} else if(split_mes[0] == "/connections") {
				std::cout << "========== Total connections: " << server.Connections() << std::endl;
			} else if((split_mes[0] == "/close") && (split_mes.size() >= 2)) {
				if(is_numeric(split_mes[1], 'i')) {
					int num = patch::stoi(split_mes[1]);
					server.RemoveClient(num, true);
				} else {
					server.RemoveClient(server.IDFromName(split_mes[1]), true);
				}
			} else if((split_mes[0] == "/mes") && (split_mes.size() >= 3)) {
				if(is_numeric(split_mes[1], 'i')) {
					int num = patch::stoi(split_mes[1]);
					server.SendMessage(split_mes[2], num);
				} else {
					server.SendMessage(split_mes[2], server.IDFromName(split_mes[1]));
				}
			} else if((split_mes[0] == "/stats") && (split_mes.size() >= 2)) {
				if(is_numeric(split_mes[1], 'i')) {
					int num = patch::stoi(split_mes[1]);
					server.ShowUserData(num);
				} else {
					server.ShowUserData( server.IDFromName(split_mes[1]) );
				}
			} else if(split_mes[0] == "/test") {
				for(unsigned int i = 0; i < split_mes.size(); i++) {
					std::cout << split_mes[i] << "\n";
				}
			} else if((split_mes[0] == "/hurt") && (split_mes.size() >= 3)) {
				if(is_numeric(split_mes[1], 'i') && is_numeric(split_mes[2], 'i')) {
					int num = patch::stoi(split_mes[1]);
					int dmg = patch::stoi(split_mes[2]);

					server.Hurt(num, dmg);
				}
			}
		}
	}

	server.Shutdown();

    return (0);
}
