#pragma once
// Socket connections are made here

// openssl libaries are used for SSL encryption
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/types.h>

#include <stdio.h>
#include <thread>
#include <string>
#include<iostream>
#include<memory>

#include "request.hpp"
#include "../db.hpp"

// NOTE: For future windows port
#ifndef _WIN32
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>

#define CLOSE_SOCKET(SOCKET) shutdown(SOCKET, SHUT_RDWR); close(SOCKET)
#else

    #define WIN32_LEAN_AND_MEAN

    #include <winsock2.h>
    #include <windows.h>
    #include <Ws2tcpip.h>

#define CLOSE_SOCKET(SOCKET) closesocket(SOCKET)
#endif

// Max amount of bytes a HTTP request can contain
#define CLIENT_REQUEST_SIZE 8192

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
            server(std::string ip, short port, bool ssl = false,
                    std::string certificate = "", std::string key = "",
                    const char *db_user = "root", const char *db_password = "", const char *db_name = "cloudpotato");
            ~server();

            void start();
            void stop();
            void stop_listening();
            void listen_handler();
            void ssl_listen_handler();

    };

    // For multi-threading purposes
    void server_starter(server *server_ptr);

}

void reply(request_handler::request *req, struct networking::client_stream, db_handler::db *db_connection);

