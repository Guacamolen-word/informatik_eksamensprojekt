
#include "db.hpp"

using db_handler::db;

bool db_handler::is_prohibited_character(char c)
{ if(c == '\n' || c == '\r' || c == ' ' || c == ';') {return true;} return false; }

void db_handler::clean_query(std::string &query) {
    (void)std::remove_if(query.begin(), query.end(), is_prohibited_character);
}


// Connect to DB
db::db(std::string server_name, std::string username, std::string password, std::string database) {

    this->connection = mysql_init(NULL);

    if(mysql_real_connect(this->connection,
                server_name.c_str(),
                username.c_str(), password.c_str(),
                database.c_str(),
                // TODO: fix unexplained constants
                0, NULL, 0) != NULL) {
        this->connected = true;
    }else {
        this->connected = false;
    }
}

db::~db() {
    mysql_close(this->connection);
}


enum errors db::query(std::string qur) {
    
    if(util::contains_terminating_char(qur) == true ||
       mysql_query(this->connection, qur.c_str()) != 0) {
        return DB_ERROR;
    }

    return OK;
}

enum errors db::store_result() {
    this->result = mysql_store_result(connection);

    if(this->result == NULL)
        return DB_ERROR;
    return OK;
}

MYSQL_ROW db::get_row() {
    return this->row;
}

void db::next_row() {
    this->row = mysql_fetch_row(this->result);
}

// Generate new session for user
enum errors db::new_session(std::string username, std::string &out) {

    // Ensure user is valid
    if(!this->is_valid_user(username)) {
        return DB_ERROR;
    }

    // Generate session ID
    std::string id = uuids::to_string (uuids::uuid_system_generator{}());

    // Add session ID to DB
    if(mysql_query(this->connection, 
            ("UPDATE users SET session = '" + id + "' WHERE username = '" + username + "'").c_str()
            ) != 0) {
        return DB_ERROR;
    }

    // Output session ID via reference so it can be returned to
    // the client as a cookie
    out = id;
    return OK;
}

// Remove session from DB
enum errors db::remove_session(std::string username) {

    if(!this->is_valid_user(username)) {
        return DB_ERROR;
    }

    if(mysql_query(this->connection, 
            ("UPDATE users SET session = NULL WHERE username = '" + username + "'").c_str()
            ) != 0) {
        return DB_ERROR;
    }

    return OK;
}

// Check if a user exists
bool db::is_valid_user(std::string &username) {
    MYSQL_RES *result;
    MYSQL_ROW row;

    if(mysql_query(this->connection,
                ("SELECT username FROM users WHERE username = '" + username + "'").c_str()) != 0) {
        return false;
    }

    result = mysql_store_result(this->connection);
    if(result == NULL)
        return false;

    row = mysql_fetch_row(result);

    if(std::string(row[0]) == username)
        return true;
    return false;

}

// Check whether if a user's session is genuine or not
bool db::is_valid_session(std::string username, std::string session) {

    MYSQL_RES *result;
    MYSQL_ROW row;

    if(mysql_query(this->connection,
                ("SELECT session FROM users WHERE username = '" + username + "'").c_str()
                ) != 0) {
        return false;
    }

    result = mysql_store_result(this->connection);
    if(result == NULL)
        return false;

    row = mysql_fetch_row(result);

    if(row[0] != NULL && std::string(row[0]) == session) {
        return true;
    }

    return false;
}

bool db::is_connected() {
    return this->connected;
} 


