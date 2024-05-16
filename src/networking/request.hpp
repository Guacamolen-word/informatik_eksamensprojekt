 //[ Adam ]
#pragma once 
// This namespace is used for parsing HTTP requests.
// Currently it only parses the bare essentials e.g.
// cookies, requested URL and x-www-form-urlenconded 
// POST requests. Any other POST formats will fail

#include <cstdlib>
#include <iostream>
#include "cookie.hpp"

namespace request_handler {

    // Internal numerical reprensentation for parsing GET/POST token 
    enum type { GET = 0, POST = 1, ERROR_TYPE = 2};
    enum type str_to_type(std::string);
    bool key_exists(const char *key, std::unordered_map<std::string, std::string> &hashmap);

    // NOTE: No longer in use
    struct post{
        std::string name, value;
    };

    // For parsing HTTP requests values e.g.
    // Cookie: name=value
    // Content-Type: application/x-www-form-urlencoded
    //
    // NOTE: Unused right now
    enum request_value_type { COOKIE, CONTENT_TYPE, CONTENT_LENGTH, POST_DATA };
    struct request_value {
        std::string_view name, value;
        enum request_value_type type;
    };

    struct request {
        private:
            void get_post(std::string post_raw);
            void parse_request(std::string_view request);
        public:
            // Is set to false in case of error
            bool valid;
            enum type request_type;
            std::string url;

            std::unordered_map<std::string, std::string> posts;
            std::unordered_map<std::string, std::string> cookies;

            request(char *http_request);
    };

} 
