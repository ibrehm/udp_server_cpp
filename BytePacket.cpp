// (C) 2018 Ian Brehm
//-----------------------------------------------------------------------------------------------------------
#include "BytePacket.h"

//--------------------------------------------------------------------------
// Initialize variables
BytePacket::BytePacket() {

	iterator = DATA_START;
	giterator = 0;
	total_var = 0;
	data.assign(ByteSize, '\0');
}
//--------------------------------------------------------------------------
// Was originally used, but may not be needed in the future, might remove.
BytePacket::~BytePacket() {

}
//--------------------------------------------------------------------------
// Returns all the data in the packet with the proper header information
const char* BytePacket::rtrnData() {

	// Store the variable total in a string buffer
	std::string add_total(reinterpret_cast<char*>(&total_var), sizeof(int));

	// Add the total to the overall buffer
	for(unsigned int i = 0; i < sizeof(int); i++) {
		data[i+VAR_TOTAL_START] = add_total[i];
	}
	// Set the chunk data to 0. Isn't used yet.
	data[CHUNK_START] = 0;
	data[CHUNK_TOTAL_START] = 0;

	return (data.data());
}
//--------------------------------------------------------------------------
// Shows how many bytes are used in the buffer thus far.
int BytePacket::Size() {
	return (iterator);
}
//--------------------------------------------------------------------------
int BytePacket::Remaining() {
	return(ByteSize - iterator);
}
//--------------------------------------------------------------------------
// Sets the command and ID of the packet
void BytePacket::SetIntent(int cmd, int id) {
	Clear();

	// Store the ID and CMD in a string buffer
	//std::string adding_cmd((char*)&cmd, sizeof(cmd));
	std::string adding_cmd(reinterpret_cast<char*>(&cmd), sizeof(cmd));
	std::string adding_id(reinterpret_cast<char*>(&id), sizeof(id));

	// Add it to the overall data
	for(unsigned int i = 0; i < sizeof(int); i++) {
		data[(i+CMD_START)] = adding_cmd[i];
		data[(i+ID_START)] = adding_id[i];
	}
}
//--------------------------------------------------------------------------
// General Add function, mostly called by the templated version.
void BytePacket::Add(const char* adding, char size) {

	// Only add data if we got the space
	if(Remaining() >= (size+1)) {
		// Store the size of the data (Strings must be kept to 255 in length)
		data[iterator] = size;
		iterator++;
		// Add the data to the overall buffer
		for(int i = 0; i < size; i++) {
			data[iterator] = adding[i];
			iterator++;
		}
		// Increase the variable total
		total_var++;
	}
}
//--------------------------------------------------------------------------
// Add a buffer to the packet
void BytePacket::Append(char* append, int length) {
	// Clear it first to reset
	Clear();
	iterator = length;
	// Add data as needed
	for(int i = 0; i < length; i++) {
		if(append[i] != '\0') {
			data[i] = append[i];
		}
	}
}
//--------------------------------------------------------------------------
// Deletes all the variables within the packet.
void BytePacket::Empty() {
	for(int i = DATA_START; i < iterator; i++) {
		data[i] = '\0';
	}
	iterator = DATA_START;
	giterator = 0;
	total_var = 0;
}
//--------------------------------------------------------------------------
// Completely resets a packet
void BytePacket::Clear() {
	// Set all characters to be blank
	for(int i = 0; i < iterator; i++) {
		data[i] = '\0';
	}
	// Reset variables
	iterator = DATA_START;
	giterator = 0;
	total_var = 0;
}
//--------------------------------------------------------------------------
