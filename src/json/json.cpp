 //[ Adam ]
 
#include "json.hpp"

using json_handler::json;

json::json() {
    this->formatted_json = "{\n";
}

json::~json() {}


void json::add(struct json_field value) {

    this->formatted_json += ",\n\"" +
        value.name + "\": " + value.value;

}

void json::add(const char *field, const char *name) {
    (void)field;
    (void)name;
}

void json::add_pair(std::vector<struct json_field> json ) {
    this->formatted_json[ this->formatted_json.length() - 1 ] = ',';
    this->formatted_json += "{";

    for(auto i : json) {
      this->formatted_json += "\"" + i.name + "\": " + i.value + ", "; 
    }

    this->formatted_json += "}}";
}

std::string_view json::get_formatted_json() {

    std::string_view t = formatted_json;

    t = t.substr(0, t.length() - 2);

    return t;
}



