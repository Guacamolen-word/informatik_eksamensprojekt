#pragma once
// Socket connections are made here

// openssl libaries are used for SSL encryption
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/types.h>

// NOTE: For future windows port
#ifndef _WIN32
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#else

    #define WIN32_LEAN_AND_MEAN

    #include <winsock2.h>
    #include <windows.h>
    #include <Ws2tcpip.h>

#endif

// Max amount of bytes a HTTP request can contain
#define CLIENT_REQUEST_SIZE 8192

#include <stdio.h>
#include <thread>
#include <string>
#include<iostream>
#include<memory>

#include "request.hpp"
#include "../db.hpp"


namespace networking {

    // Once a connection between a client and the server has been
    // established, the client handle will be passed to one of these
    // functions
    void client_handler(int client, db_handler::db *db_connection);
    void ssl_client_handler(int client, db_handler::db *db_connection, 
            std::string certificate, std::string key);

    // Client handle
    struct client_stream {
        bool ssl;
        union {
            FILE *tcp;
            SSL *https;
        };
    };

    class server {
        private:
            int server_socket;
            int tmp_client;
            struct sockaddr_in info;

            bool running, listening, ssl;
            std::string certificate, key;

            db_handler::db *db_connection;

        public:
            server(std::string ip, short port, bool ssl,
                    std::string certificate, std::string key );
            ~server();

            void start();
            void listen_handler();
            void ssl_listen_handler();

    };

}

void reply(request_handler::request *req, struct networking::client_stream, db_handler::db *db_connection);

