 //[ Adam ]
#pragma once 

#include <string>
#include <vector>

namespace json_handler {

    struct json_field {
        std::string name, value;
    };

    class json {
        private:
            std::vector<struct json_field> values;
            std::string formatted_json; 
//            bool done;

        public:
            json();
            ~json();

            void add(struct json_field value);
            void add(const char *field, const char *name);
            void add_pair(std::vector<struct json_field> json );

            std::string_view get_formatted_json();

    };
}
