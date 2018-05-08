#ifndef __utils_h__
#define __utils_h__

#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

//--------------------------------------------------------------------------------------------
// In some MingW/g++ compilers, the std::to_string function does not work due to a bug. This
// is a "patch" work-a-round. Same issue with stoi.
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }

    int stoi(std::string);
}
//--------------------------------------------------------------------------------------------
// Splits a single string into an array of strings based on the character passed to the 2nd
// argument.
std::vector<std::string> splitStr(std::string str, char split = ' ', char ignoreSwitch = '`');
//--------------------------------------------------------------------------------------------
// This function returns true or false based on the data within a string. If the characters in
// a given string are numeric, it'll return true. If the programmer wishes to return true only
// for floats or integers, they need to pass 'i' or 'f' for the second argument.
bool is_numeric(std::string str, char only = '*');
//--------------------------------------------------------------------------------------------
float Distance(float, float, float, float);
//--------------------------------------------------------------------------------------------
bool inArea(float posx, float posy, float x1, float y1, float x2, float y2);
//--------------------------------------------------------------------------------------------

#endif
