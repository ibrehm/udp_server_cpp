// (C) 2018 Ian Brehm
//-----------------------------------------------------------------------------------------------------------
// This header file is used to handle small variations between winsock (windows) and
// Berkeley Sockets in Linux systems, to allow better cross compatability.

#ifndef __cross_network_h__
#define __cross_network_h__

#include <iostream>
#include <unistd.h>

#ifdef _WIN32
	#include<winsock2.h>
#else
	#include <sys/socket.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	typedef int SOCKET;

	inline void closesocket(SOCKET socket) {
		close(socket);
	}
#endif // _WIN32

// Inline function overrides for compatability between Windows and Linux. Linux libraries want Unsigned Integers,
// while Windows wants regular integers.
inline ssize_t recvfrom(SOCKET socket, char* buffer, int size, struct sockaddr *addr, void* c, int flags = 0) {


	#ifdef _WIN32
		ssize_t recv_length = recvfrom(socket, buffer, size, flags, addr, (int*)c);
	#else
		ssize_t recv_length = recvfrom(socket, buffer, size, flags, addr, (unsigned int*)c);
	#endif // _WIN32

	return(recv_length);

}

inline SOCKET accept(SOCKET socket, struct sockaddr *addr, void* c) {
	#ifdef _WIN32
	SOCKET rtrn = accept(socket, addr, (int*)c);
	#else
	SOCKET rtrn = accept(socket, addr, (unsigned int*)c);
	#endif // _WIN32
	return rtrn;
}

// Windows needs the WSA stuff to start up before any socket stuff can be done.
// This object is designed to be included in both Windows and non-Windows code,
// but it'll do nothing if the OS isn't Windows.
class WSAStart {
public:
	WSAStart() {
		#ifdef _WIN32
		int iResult;
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
			std::cout << "WSAStartup failed with error: " << iResult << "\n";
		}
		#endif // _WIN32
	}
	~WSAStart() {
		#ifdef _WIN32
		WSACleanup();
		#endif // _WIN32
	}
private:
	#ifdef _WIN32
	WSADATA wsaData;
	#endif // _WIN32
};

#endif // __cross_network_h__
