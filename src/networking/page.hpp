 //[ Adam ]
#pragma once 
// Used for building the HTTP reesponse the user gets

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <iterator>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "cookie.hpp"
#include "server.hpp"
#include "errors.hpp"

using cookie_handler::cookie;


namespace response_handler {

    enum filetype {HTML = 0, CSS, JS, JSON, PNG, UNKNOWN};
    enum filetype parse_filetype(std::string_view url);

    enum errors read_file(std::string file_path, std::string &out);
    enum errors read_binary_file(std::string file_path, std::vector<char> &out);

    class page{
        private:
            enum filetype type;
            enum errors status_code;

            std::string header, footer;
            std::vector<struct cookie> cookies;
            bool binary_file;

#ifndef _WIN32
            void print_page(FILE *client_stream);
#else
            void print_page(int client_stream);
#endif
            void print_page_ssl(SSL *c_ssl);
        public:
            page(std::string &url);
            ~page();

            std::vector<char> binary;
            std::ostringstream body;

            void set_header(std::string);
            void set_footer(std::string);
            void add_cookie(struct cookie new_cookie);
            enum filetype get_filetype();
            void set_status_code(enum errors);
            enum errors get_status_code();
            void set_filetype(enum filetype type);

            void response(struct networking::client_stream client);

    };

};

