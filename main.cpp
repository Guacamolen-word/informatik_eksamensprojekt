
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
        std::cout << "Fetching all data\n";
        db->query("SELECT time," + req->posts["filter"] + " FROM data");

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
        std::cout << "Time: " << row[0] << ", Value: " << row[1];
        response_page->body << "{\"value\": " << row[1] << ", \"time\": " << row[0] << "},";
  
        db->next_row(); row = db->get_row();
    }
    std::cout << "\n";
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

int main(int argc, char **argv) {
    std::signal(SIGINT, signal_handler);

    gtk_init(&argc, &argv);
    gui::load_gui();

    /*
    std::unique_ptr<networking::server> server (new networking::server("10.0.2.15", 8080, true, 
                "./certs/cert.pem", "./certs/key.pem"));
    server->start();
*/
    return 0;
}


void reply(request_handler::request *req,
        struct networking::client_stream client,
        db_handler::db *db_connection ) {

    std::unique_ptr<page> new_page (new page( req->url ));

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
                break;
         }
    }else if(req->url == "/graph") {
        graph(std::move(new_page),
                req, db_connection, client);
    }else if(req->url == "/data" ) {
        switch (req->request_type) {
            case GET:
                get_data(std::move(new_page), req, db_connection, client);
                break;
            case POST:
                get_data(std::move(new_page), req, db_connection, client);
                break;
            default:
                new_page->set_status_code(INTERNAL_SERVER_ERROR);
                new_page->response(client);
                break;
        }
    }else if(req->url == "/logout" ) {
        logout(std::move(new_page), req, db_connection, client);
    }else {
        new_page->set_status_code(NOT_FOUND);
        new_page->response(client);
    }

}


