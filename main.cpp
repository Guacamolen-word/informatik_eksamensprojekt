// [ Adam ]

// NOTE: Super hedious test file. This exists for the purpose of just testing
// all of the code that has been written. All of this needs to be re-written

#include "src/networking/webserver.hpp"
#include "src/gui/gui.hpp"
#include <csignal>

#define VALDIDATE_SESION()\
    if(!request_handler::key_exists("username", req->cookies) &&\
       !request_handler::key_exists("session", req->cookies)  &&\
       !db->is_valid_session(req->cookies["username"], req->cookies["session"]))\
    {\
        response_page->set_status_code(UNAUTHORIZED);\
        response_page->response(client);\
        return;\
    }

#define TEAPOT_EXIT()\
{\
    response_page->set_status_code(IM_A_TEATPOT);\
    response_page->response(client);\
    return;\
}

#define ERROR_EXIT()\
{\
    response_page->set_status_code(INTERNAL_SERVER_ERROR);\
    response_page->response(client);\
    return;\
}


void about(std::unique_ptr<page> response_page, 
              request_handler::request *req,
              db_handler::db *db,
              struct networking::client_stream client) {

    VALDIDATE_SESION();

    response_page->set_header("html/header.html");
    response_page->set_footer("html/footer.html");

    std::string body_content; response_handler::read_file("html/about.html", body_content);
    response_page->body << body_content;

    response_page->response(client);

}

void home(std::unique_ptr<page> response_page, 
              request_handler::request *req,
              db_handler::db *db,
              struct networking::client_stream client) {

    VALDIDATE_SESION();

    response_page->set_header("html/header.html");
    response_page->set_footer("html/footer.html");

    std::string body_content; response_handler::read_file("html/home.html", body_content);
    response_page->body << body_content;

    response_page->response(client);

}

void raw_data(std::unique_ptr<page> response_page, 
              request_handler::request *req,
              db_handler::db *db,
              struct networking::client_stream client) {

    VALDIDATE_SESION();

    response_page->set_filetype(JSON);

    db->query("SELECT time,temperature,humidity FROM data");

    db->store_result(); db->next_row();
    MYSQL_ROW row = db->get_row();

    if(row == NULL) {
        response_page->body << "[ { \"message\": \"No data found in SQL database\"  }  ]";
        response_page->response(client);
        return;
    }

    response_page->body << "[";
    while (row != NULL) {
        response_page->body << "{ \"time\": " << row[0] << ", \"temperature\": " << row[1] << ", \"humidity\": " << row[2] << "},";
        db->next_row();
        row = db->get_row();
    }

    // Override extra comma at the end of the string with ']'
    response_page->body.seekp(-1, std::ios_base::end); 
    response_page->body << "]";

    response_page->response(client);

}

void upload(std::unique_ptr<page> response_page, 
              request_handler::request *req,
              db_handler::db *db,
              struct networking::client_stream client) {


    if(!request_handler::key_exists("time", req->posts) ||
       !request_handler::key_exists("humidity", req->posts) ||
       !request_handler::key_exists("temp", req->posts) ) 

    {
        TEAPOT_EXIT();
    }

//    std::cout << "Got data: time = " 
//              << req->posts["time"] 
//              << ", and temp = " 
//              << req->posts["temp"] << "\n";

    db->query("INSERT INTO data(time, temperature, humidity) VALUES(" + req->posts["time"] + "," + req->posts["temp"] + "," + req->posts["humidity"] + ")");

    response_page->set_status_code(OK);
    response_page->body << "Tak for data - Adam :)\n";
    response_page->response(client);
}


void graph(std::unique_ptr<page> response_page, 
              request_handler::request *req,
              db_handler::db *db,
              struct networking::client_stream client) {

    VALDIDATE_SESION();

    response_page->set_header("html/header.html");
    response_page->set_footer("html/footer.html");

    std::string body_content; response_handler::read_file("html/graphs.html", body_content);
    response_page->body << body_content;

    response_page->response(client);
}

void get_data(std::unique_ptr<page> response_page, 
              request_handler::request *req,
              db_handler::db *db,
              struct networking::client_stream client) {

    VALDIDATE_SESION();

    if(!request_handler::key_exists("filter", req->posts))
        ERROR_EXIT();

    response_page->set_filetype(JSON);

    if( !request_handler::key_exists("time_a", req->posts) && !request_handler::key_exists("time_b", req->posts) ) {
        //std::cout << "Fetching all data\n";
        db->query("SELECT time," + req->posts["filter"] + " FROM data ORDER BY time ASC");

    }else if(request_handler::key_exists("time_a", req->posts) && request_handler::key_exists("time_b", req->posts)) {
        if( !util::is_int(req->posts["time_a"]) || !util::is_int(req->posts["time_b"])  )
            TEAPOT_EXIT();

        int time_a = std::atoi( req->posts["time_a"].c_str() );
        int time_b = std::atoi( req->posts["time_b"].c_str() );

        if(time_a == 0)
            db->query("SELECT time," + req->posts["filter"] + " FROM data WHERE time <= " + req->posts["time_b"]);
        else if (time_b == 0) 
            db->query("SELECT time," + req->posts["filter"] + " FROM data WHERE time >= " + req->posts["time_a"]);
        else
            db->query("SELECT time," + req->posts["filter"] + " FROM data WHERE time >= " + req->posts["time_a"] + " AND time <= " + req->posts["time_b"]);

    }else
        TEAPOT_EXIT();

    db->store_result(); db->next_row();
    MYSQL_ROW row = db->get_row();

    response_page->body << "[";
    while (row != NULL) {
//        std::cout << "Time: " << row[0] << ", Value: " << row[1];
        response_page->body << "{\"value\": " << row[1] << ", \"time\": " << row[0] << "},";
  
        db->next_row(); row = db->get_row();
    }
    //std::cout << "\n";
//    response_page->body.seekp(-1,response_page->body.cur); 
    response_page->body.seekp(-1, std::ios_base::end); 
    response_page->body << "]";

    response_page->response(client);

}

void logout(std::unique_ptr<page> response_page,
            request_handler::request *req,
            db_handler::db *db, struct networking::client_stream client) {  

    if(request_handler::key_exists("username", req->cookies))
        db->remove_session(req->cookies["username"]);

    response_page->set_header("html/login_header.html");
    response_page->set_footer("html/login_footer.html");

    std::string login_page;
    read_file("html/login.html", login_page);
    response_page->body << login_page;

    response_page->response(client);
}

void login_page_get(std::unique_ptr<page> response_page,
                    request_handler::request *req,
                    db_handler::db *db, struct networking::client_stream client) {

    if(request_handler::key_exists("username", req->cookies) && 
       request_handler::key_exists("session", req->cookies)  && 
       db->is_valid_session(req->cookies["username"], req->cookies["session"])) {

        response_page->set_header("html/header.html");
        response_page->set_footer("html/footer.html");
        response_page->body << "welcome admin<br>\n";
        response_page->body << "<a href=\"/logout\">logout</a><br>\n";

    }else {
        response_page->set_header("html/login_header.html");
        response_page->set_footer("html/login_footer.html");

        std::string login_page;
        read_file("html/login.html", login_page);
        response_page->body << login_page;
    }


    response_page->response(client);
}

void login_page_post(std::unique_ptr<page> response_page,
                     request_handler::request *req,
                     db_handler::db *db, struct networking::client_stream client) {

    db->query("SELECT password FROM users WHERE username = '" + req->posts["username"] + "'");
    db->store_result(); db->next_row();

    MYSQL_ROW row = db->get_row();

    if(request_handler::key_exists("password", req->posts) &&
            request_handler::key_exists("username", req->posts)) {

//        std::cout << "Got user: " << req->posts["username"] << "got pass: " << req->posts["password"] << "\n";
//        std::cout << "row: " << std::string(row[0]) << std::endl;

        if( row != NULL && req->posts["password"] == std::string(row[0])  ) {
            response_page->set_header("html/header.html");
            response_page->set_footer("html/footer.html");
            response_page->body << "welcome admin<br>\n";
            response_page->body << "<a href=\"/logout\">logout</a><br>\n";

            std::string id;
            db->new_session(req->posts["username"], id);
            response_page->add_cookie((struct cookie){ "session", id });
            response_page->add_cookie((struct cookie){ "username", req->posts["username"] });
        }else {
            response_page->set_header("html/login_header.html");
            response_page->set_footer("html/login_footer.html");
            response_page->body << "who tf r u?<br>\n";
            std::string login_page;
            read_file("html/login.html", login_page);
            response_page->body << login_page;
        }
    }

    response_page->response(client);

}

void error_response(std::unique_ptr<page> response_page,
                     request_handler::request *req,
                     db_handler::db *db, struct networking::client_stream client) {

    (void)req;
    (void)db;
    response_page->set_status_code(INTERNAL_SERVER_ERROR);
    response_page->response(client);

}


void signal_handler(int sig) {
    exit(sig);
}

void print_help() {
    std::cout << "Usage: cloudpotato [OPTION] [ARG]\n\n"
              << "Options:\n"
              << "--gui\t\tStart GUI control panel"
              << "\n\nCLI-mode only:\n"
              << "--ip [IP]\t\tSpecify IP to bindh\n"
              << "--port [PORT]\t\tSpecify port to use\n"
              << "--https [CERT] [KEY]\t\tEnable HTTPS\n"
              << std::endl;
}

int main(int argc, char **argv) {
    std::signal(SIGINT, signal_handler);

#ifndef _WIN32
    if(argc < 2) {
        print_help();
        return 0;
    }

    if( std::strcmp( argv[1], "--gui"  ) == 0 ) {
#endif
    gtk_init(&argc, &argv);
    gui::load_gui();
#ifdef WIN32
    return 0;
#endif

#ifndef _WIN32
    }

    int ip = 0, port = 0, cert = 0, key = 0;
    bool ssl = false;

    for (int i = 1; i < argc; i++) {

        if( std::strcmp(argv[i], "--ip") == 0  ) {
            ip = i + 1;
            i++;
        }else if( std::strcmp(argv[i], "--port") == 0  ) {
            port = i + 1;
            i++;
        }else if( std::strcmp(argv[i], "--https") == 0  ) {
            ssl = true;
            cert = i + 1;
            key = cert + 1;
            i += 2;
        }

    }

    if(!util::is_int(argv[port])) {
        std::cout << "Port must be number" << std::endl;
        return 0;
    }

    std::unique_ptr<networking::server> server;

    if(ssl) {
        std::cout << "ip: " << argv[ip] << "port: " << argv[port] << "cert: " << argv[cert] << ", " << argv[key] << std::endl;
        server = std::unique_ptr<networking::server>(new networking::server(
                    argv[ip], std::atoi(argv[port]), true, argv[cert], argv[key]));
    }else {
        server = std::unique_ptr<networking::server>(new networking::server(
                    argv[ip], std::atoi(argv[port]), false, " ", " "));
    }

    server->start();

    return 0;
#endif
}


void reply(request_handler::request *req,
        struct networking::client_stream client,
        db_handler::db *db_connection ) {

    std::unique_ptr<page> new_page (new page( req->url ));

    // If the request url is a file, then simply return the file
    // otherwise call one of the dynamic page functions
    if(new_page->get_filetype() != HTML) {
        new_page->response(client);
        return;
    }

    if( req->url == "/login" || req->url == "/" )  {

        switch (req->request_type) {
            case GET:
                login_page_get(std::move(new_page), req, db_connection, client);
                break;
            case POST:
                login_page_post(std::move(new_page), req, db_connection, client);
                break;
            default:
                new_page->set_status_code(INTERNAL_SERVER_ERROR);
                new_page->response(client);
                break;
         }

    } else if(req->url == "/data" ) {
        switch (req->request_type) {
            case GET:
                raw_data(std::move(new_page), req, db_connection, client);
                break;
            case POST:
                get_data(std::move(new_page), req, db_connection, client);
                break;
            default:
                new_page->set_status_code(INTERNAL_SERVER_ERROR);
                new_page->response(client);
                break;
        }
    } else if(req->url == "/upload"){
        upload(std::move(new_page), req, db_connection, client);
    } else if (req->url == "/about") {
        about(std::move(new_page), req, db_connection, client);
    } else if (req->url == "/home") {
        home(std::move(new_page), req, db_connection, client);
    } else if(req->url == "/graph") {
        graph(std::move(new_page), req, db_connection, client);
    }else if(req->url == "/logout" ) {
        logout(std::move(new_page), req, db_connection, client);
    }else {
        new_page->set_status_code(NOT_FOUND);
        new_page->response(client);
    }

}


