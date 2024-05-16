 //[ Adam ]
 
#include "util.hpp"

bool util::contains_terminating_char(std::string_view str) {
    for(char c : str) {
        if(c == '\n' || c == '\r' || c == ';')
            return true;
    }

    return false;
}

bool util::is_int(std::string_view str) {
    for(char c : str) {
        if(!std::isdigit(c))
            return false;
    }

    return true;
}
