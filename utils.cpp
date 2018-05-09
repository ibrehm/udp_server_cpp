// (C) 2018 Ian Brehm
//-----------------------------------------------------------------------------------------------------------
#include <cmath>
#include "utils.h"

int patch::stoi(std::string s) {
	int rtrn = 0;

	rtrn = strtol(s.c_str(),0,10);

	return rtrn;
}

//--------------------------------------------------------------------------------------------
// Splits a single string into an array of strings based on the character passed to the 2nd
// argument.
std::vector<std::string> splitStr(std::string str, char split, char ignoreSwitch) {

	bool pushed = false;
	bool ignore = false;
	std::vector<std::string> rtrn;
	std::string push;

	rtrn.push_back(push);

	for(unsigned int i = 0; i < str.size(); i++) {

		if(str[i] == ignoreSwitch) {
			ignore = !ignore;
			continue;
		}

		if((str[i] == split) && (ignore != true)) {
			if(pushed == false) {
				rtrn.push_back(push);
				pushed = true;
			}
			continue;
		} else {
			pushed = false;
			rtrn[rtrn.size()-1] += str[i];
		}

	}

	return(rtrn);

}
//--------------------------------------------------------------------------------------------
// This function returns true or false based on the data within a string. If the characters in
// a given string are numeric, it'll return true. If the programmer wishes to return true only
// for floats or integers, they need to pass 'i' or 'f' for the second argument.
bool is_numeric(std::string str, char only) {
	int decimal = 0;

	if( ((str[0] == '-') && (str.size() == 1)) || (str.size() == 0)) {
		return (false);
	}
	for(unsigned int i = 0; i < str.size(); i++) {

		if( (str[i] < '0') || (str[i] > '9') ) {
			if((i == 0) && (str[i] == '-')) {
				continue;
			} else if( (str[i] == '.') && (decimal == 0) ) {
				decimal++;
				continue;
			}
			return (false);
		}

	}
	if((only == 'i') && (decimal > 0)) {
		return (false);
	} else if((only == 'f') && (decimal == 0)) {
		return (false);
	}

	return(true);

}
//--------------------------------------------------------------------------------------------
float Distance(float x1, float y1, float x2, float y2) {
	float rtrn = sqrt( (pow( (x2-x1) , 2) + pow( (y2-y1) , 2)) );
	return(rtrn);
}
//--------------------------------------------------------------------------------------------
bool inArea(float posx, float posy, float x1, float y1, float x2, float y2) {
	bool rtrn = false;

	if((posx >= x1) && (posx <= x2)) {
		if((posy >= y1) && (posy <= y2)) {
			return(true);
		}
	}

	return(rtrn);
}
//--------------------------------------------------------------------------------------------









