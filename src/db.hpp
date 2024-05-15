#pragma once
// This namespace is used for handling connections to a (local) MariaDB or MySQL database
// TODO: Check for ; or \n/\r in query variables to fix SQL injection exploit

#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include <mutex>
#include <algorithm>

#include "networking/errors.hpp"
#include "util.hpp"

#ifdef _WIN32
#include<combaseapi.h>
#endif

// Library for generating cryptographically secure session IDs
#define UUID_SYSTEM_GENERATOR
#include "uuid.hpp"

namespace db_handler {

    bool is_prohibited_character(char c);
    void clean_query(std::string &query);

    class db {
        private:
            MYSQL *connection;
            MYSQL_RES *result;
            //unsigned long columns;
            MYSQL_ROW row;

            bool connected;
        public:
            db(std::string server_name, std::string username, std::string password, std::string database);
            ~db();

            enum errors query(std::string qur); 
            enum errors store_result();
            MYSQL_ROW get_row();
            void next_row();

            enum errors new_session(std::string username, std::string &out);
            enum errors remove_session(std::string username);
            bool is_valid_session(std::string username, std::string session);
            bool is_valid_user(std::string &username);
            bool is_connected();

            std::mutex db_mutex; 
    };

}

