 //[ Adam ]
 
#include "server.hpp"

using networking::server;

// Read HTTP request and generate response over secure SSL encrypted connection
void networking::ssl_client_handler(int client, db_handler::db *db_connection, 
        std::string certificate, std::string key) {

    // Prepare SSL socket
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_server_method());
    SSL_CTX_set_options(ssl_ctx, SSL_OP_SINGLE_DH_USE);
    (void)SSL_CTX_use_certificate_file(ssl_ctx, certificate.c_str(), SSL_FILETYPE_PEM);
    (void)SSL_CTX_use_PrivateKey_file(ssl_ctx, key.c_str(), SSL_FILETYPE_PEM);

    SSL *c_ssl = SSL_new(ssl_ctx);
    SSL_set_fd(c_ssl, client);

    if(SSL_accept(c_ssl) <= 0) {
        std::cout << "SSL failed to accept\n";
        return;
    }

    // Allocate memory for HTTP request
    char *buffer = (new char[CLIENT_REQUEST_SIZE]);
    
    // Read HTTP request from client
    ssize_t req_size = SSL_read(c_ssl, buffer, CLIENT_REQUEST_SIZE);

    // If nothing was requested(or an error occured)
    // then free allocated resources
    if(req_size <= 0) {
        std::cout << "Blank request\n";

        CLOSE_SOCKET(client);

        SSL_shutdown(c_ssl);
        SSL_free(c_ssl);
        delete[] buffer;
        return;
    }
    buffer[req_size] = '\0';

    std::cout << buffer << '\n';

    // Begin parsing HTTP request
    request_handler::request *new_request = (new request_handler::request(buffer));
    struct networking::client_stream new_client_stream;
    new_client_stream.ssl = true;
    new_client_stream.https = c_ssl;

    // DB mutex to prevent race condition
    // TODO: Add support for multiple DB connections
    std::lock_guard<std::mutex> guard(db_connection->db_mutex);

    reply(new_request, new_client_stream, db_connection);

    SSL_shutdown(c_ssl);
    SSL_free(c_ssl);

    CLOSE_SOCKET(client);

    delete new_request;
    delete[] buffer;
}

// Read HTTP request and generate response over NON encrypted socket
void networking::client_handler(int client, db_handler::db *db_connection) {

    // Allocate memory for HTTP request
    char *buffer = (new char[CLIENT_REQUEST_SIZE]);
    FILE *client_stream;

    // Read HTTP request
    ssize_t req_size = recv(client, buffer, CLIENT_REQUEST_SIZE, 0);

    // Free resources in case of blank request or error
    if(req_size <= 0) {
        std::cout << "Blank request\n";

        CLOSE_SOCKET(client);

        delete[] buffer;
        return;
    }

    // Prepare client stream for replying
    client_stream = fdopen(client, "w+");
    if(client_stream == NULL) {
        std::cout << "Couldn't create client stream\n";
        delete[] buffer;
        return;
    }

    // Parse HTTP request
    request_handler::request *new_request = (new request_handler::request(buffer));
    struct networking::client_stream new_client_stream {false, {client_stream}};

    // Mutex to prevent race condition
    // TODO: Add support for multiple DB connections
    std::lock_guard<std::mutex> guard(db_connection->db_mutex);

    reply(new_request, new_client_stream, db_connection);

    fclose(client_stream);

    CLOSE_SOCKET(client);

    delete new_request;
    delete[] buffer;
}


// Init server and prepare socket
server::server(std::string ip, short port, bool ssl,
        std::string certificate, std::string key,
        const char *db_user, const char *db_password, const char *db_name) {

    // Prepare openssl library if SSL encryption is enabled
    if(ssl) {
        SSL_load_error_strings();
        SSL_library_init();
        OpenSSL_add_all_algorithms();

        this->certificate = certificate;
        this->key = key;
    }

#ifdef _WIN32
    WSADATA data;
    int wsa_result = WSAStartup(MAKEWORD(2, 2), &data); 
    if(wsa_result != NO_ERROR) {
	    std::cout << "WSA startup error\n";
	    exit(-1);
    }
#endif

    // Creates TCP socket
    this->server_socket = socket(AF_INET, SOCK_STREAM, 0);

// Socket info
    this->info.sin_family = AF_INET;
    this->info.sin_port = htons(port);
    this->info.sin_addr.s_addr = inet_addr(ip.c_str());
    // TODO: check for error

    int res = bind(this->server_socket, (const struct sockaddr*)&this->info, sizeof(this->info));
    if(res != 0) {
        std::cout << "Couldn't bind\n";
        exit(-1);
    }
    this->running = true;
    this->listening = false;
    this->ssl = ssl;

    // TODO: replace constants with variables 
    this->db_connection = (new db_handler::db("localhost", db_user, db_password, db_name));
}

// On server exit
server::~server() {
    CLOSE_SOCKET(this->server_socket);

    // Cleanup OpenSSL library resources
    if(this->ssl) {
        ERR_free_strings();
        EVP_cleanup();
    }

#ifdef _WIN32
    WSACleanup();
#endif

    delete this->db_connection;
}

void server::start() {
    if(this->ssl)
        ssl_listen_handler();
    else
        listen_handler();
}

void server::stop() {
    this->listening = false;
    CLOSE_SOCKET(this->server_socket);
}

void server::stop_listening() {
    this->listening = false;
}

// Non-SSL listener
void server::listen_handler() {

    listen(this->server_socket, 3);
    this->listening = true;

    while(this->listening) {
        this->tmp_client = accept(this->server_socket, NULL, NULL);

        std::thread t(networking::client_handler, this->tmp_client, this->db_connection);
        t.detach(); // Detatch tells the thread to exit once it's done(rather than needing to use join)
    }

}

// SSL listener
void server::ssl_listen_handler() {

    listen(this->server_socket, 3);
    this->listening = true;

    while(this->listening) {
        this->tmp_client = accept(this->server_socket, NULL, NULL);

        std::thread t(networking::ssl_client_handler, this->tmp_client, this->db_connection,
                this->certificate, this->key);
        t.detach(); // Detatch tells the thread to exit once it's done(rather than needing to use join)
    }

    std::cout << "listener stopped\n";

}

// For multi-threading purposes
void networking::server_starter(server *server_ptr) {
    server_ptr->start();
}


