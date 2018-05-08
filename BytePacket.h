#ifndef __BytePacket_h__
#define __BytePacket_h__

#include <iostream>
#include <string>
#include <type_traits>

#define ByteSize 1472

#define CMD_START 0
#define ID_START 4
#define CHUNK_START 8
#define CHUNK_TOTAL_START 9
#define VAR_TOTAL_START 10
#define DATA_START 14

enum cmd {
	CMD_NEW_PLAYER,
	CMD_QUIT_PLAYER,
	CMD_SET_NEW_ID,

	CMD_GET_SURROUNDING_ENEMIES,
	CMD_GET_SURROUNDING_PLAYERS,
	CMD_GET_SURROUNDING_TREASURE,
	CMD_GET_PLAYER_STATS,

	CMD_SHOW_POSITION,
	CMD_SEND_MESSAGE,
	CMD_INVALID_CONNECTION,

	CMD_TOTAL
};

class BytePacket {

public:
	BytePacket();
	~BytePacket();
	const char* rtrnData();
	int Size();
	int Remaining();
	void SetIntent(int, int);
	void Add(const char*, char);

	void Append(char*, int);
	void Empty();
	void Clear();

	template<typename T>
	T GetHeader(int);
	template<typename T>
	void SetHeader(int, T);
	template<typename T>
	T Get(int num = -1);
	template<typename T>
	void Add(T);
	template<typename T>
	void Update(int,T);

private:

	int iterator;
	int giterator;
	int total_var;
	std::string data;

};
//--------------------------------------------------------------------------
template<typename T>
T BytePacket::GetHeader(int start) {
	T rtrn;

	std::string temp('0', sizeof(T));
	for(unsigned int i = 0; i < sizeof(T); i++) {
		temp[i] = data[i+start];
	}
	rtrn = *(T*)temp.data();

	return(rtrn);
}
//-------------------------------------------------------------------------------------------
// This function can set and change header data. It is dangerous to use though, and may need
// to be removed later if it continues to not be needed.
template<typename T>
void BytePacket::SetHeader(int start, T adding) {
	std::string setting((char*)&adding, sizeof(T));

	for(unsigned int i = 0; i < sizeof(T); i++) {
		data[(i+start)] = setting[i];
	}
}
//-------------------------------------------------------------------------------------------
template<typename T>
void BytePacket::Add(T adding) {
	if(std::is_same<std::string, T>::value) {
		std::string *temp = (std::string*)&adding;
		int size = temp->size();
		// strings greater than 255 will need to be cut
		if(size > 255) {
			size = 255;
		}
		Add(temp->data(), size);
	} else {
		Add((const char*)&adding, sizeof(T));
	}
}
//-------------------------------------------------------------------------------------------
// This function has the capability of changing data that has already been set within the
// data packet. However, the data can only be changed if the value being added is the same
// size as the previous data it is replacing as it would corrupt data afterwards if it was
// bigger/smaller.
template<typename T>
void BytePacket::Update(int num, T adding) {
	int start = DATA_START;

	for(int i = 0; i < num; i++) {
		start+= (data[start]+1);
	}
	int size = data[start];
	start++;

	if(sizeof(T) == size) {
		std::string setting((char*)&adding, sizeof(T));
		for(unsigned int i = 0; i < sizeof(T); i++) {
			data[(start+i)] = setting[i];
		}
	}
}
//-------------------------------------------------------------------------------------------
// Can get data within the data packet. The piece of data can be specified, or can be left
// blank to obtain data in the order it was added in.
template<typename T>
T BytePacket::Get(int num) {
	T rtrn;

	if(num == -1) {
		num = giterator;
		giterator++;
	} else {
		giterator = num;
	}

	int start = DATA_START;

	// Make sure we're within range for reading
	for(int i = 0; i < num; i++) {
		start+= (data[start]+1);
	}
	int size = data[start];

	if((size+start) < Size()) {
		if(std::is_same<std::string, T>::value) {
			std::string *holder = (std::string*)&rtrn;

			for(int i = 0; i < size; i++) {
				*holder += data[start+1+i];
			}

		} else {
			if(size == sizeof(T)) {
				std::string temp('0', size);

				for(int i = 0; i < size; i++) {
					temp[i] = data[start+1+i];
				}
				rtrn = *(T*)temp.data();
			}
		}
	}

	return(rtrn);
}
//-------------------------------------------------------------------------------------------
#endif // __BytePacket_h__
