
#include "page.hpp"

using response_handler::page;
using response_handler::filetype;

enum filetype 
response_handler::parse_filetype(std::string_view url) {

    int dot_pos = url.find('.');
    if(dot_pos == -1)
        return HTML;

    std::string_view file_format = url.substr(dot_pos);
    std::cout << "file format: " << file_format << std::endl;

    if( file_format.compare(".css") == 0  ) {
        std::cout << "found css file" << std::endl;
        return CSS;
    } else if( file_format.compare(".js") == 0 ) {
        return JS;
    }else if( file_format.compare(".png") == 0 ) {
        std::cout << "found png file" << std::endl;
        return PNG;
    }

    std::cout << "Undetermined type" << std::endl;
    return UNKNOWN;
}

// Reads entire file and outputs to reference string
enum errors response_handler::read_file(std::string file_path, std::string &out) {

    if(file_path[0] == '/')
        file_path.erase(0, 1);

    std::ifstream file(file_path, std::ios::binary);

    if(!file.good()) {
        return NOT_FOUND;
        std::cout << "not found file\n";
    }

    file.seekg(0, std::ios::end);
    out.reserve((unsigned int)file.tellg());
    file.seekg(0, std::ios::beg);

    out.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());


/*
    std::ifstream file(file_path, std::ios::binary);
    std::stringstream out_stream;

    std::cout << "file_path: " << file_path << std::endl;

    if(!file.good()) {
        return NOT_FOUND;
        std::cout << "not found file\n";
    }


    out_stream << file.rdbuf();
    out = out_stream.str();
    //std::cout << "read: " << out << std::endl;
*/
    return OK;
}

// Reads binary file, e.g. image and stores it in std::vector
enum errors response_handler::read_binary_file(std::string file_path, std::vector<char> &out) {

    if(file_path[0] == '/')
        file_path.erase(0, 1);

    FILE *binary_file = fopen(file_path.c_str(), "r");

    if(binary_file == NULL) {
        return NOT_FOUND;
        std::cout << "not found file\n";
    }

    fseek(binary_file, 0L, SEEK_END);
    long pos = ftell(binary_file);
    rewind(binary_file);

    out.resize(pos);
    fread(&out[0], 1, pos, binary_file);

    fclose(binary_file);

    return OK;
}

page::page(std::string &url)
{
    this->type = parse_filetype(url);
    this->status_code = OK;

    if(this->type == PNG) {
        this->binary_file = true;
        response_handler::read_binary_file(url, this->binary);
    } else if (this->type != HTML) {
        this->binary_file = false;
        std::string file_content;
        if(response_handler::read_file(url, file_content) == OK) {
            this->body << file_content;
        }
    }
}

page::~page()
{}

// Load header file for page
void page::set_header(std::string header) {
    if(read_file(header, this->header) != OK) {
        // TODO: Handle error
    }
}

// Load footer file for page
void page::set_footer(std::string footer) {
    if(read_file(footer, this->footer) != OK) {
        // TODO: Handle error
    }
}

// Store cookie on the client's browser
void page::add_cookie(struct cookie new_cookie) {
    this->cookies.push_back(new_cookie);
}

// Output page over non encrypted socket connection
void page::print_page(FILE *client_stream) {
    std::string content_type;

    // Determine response type
    switch (this->type) {
        case HTML:
            content_type = "text/html";
            break;
        case CSS:
            content_type = "text/css";
            break;
        case PNG:
            content_type = "image/png";
            break;
        default:
            content_type = "text/html";
            break;
    }

    fprintf(client_stream, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n", content_type.c_str());

    if(this->type == HTML) {
        // Add cookies to response
        for(auto cookie : this->cookies) {
            fprintf(client_stream, "Set-Cookie: %s=%s\r\n", cookie.name.c_str(), cookie.value.c_str());
        }
    }

    fprintf(client_stream, "\r\n\r\n%s%s%s\r\n",
            this->header.c_str(), this->body.str().c_str(), this->footer.c_str() );

}

// Output page over secure SSL socket connection
void page::print_page_ssl(SSL *c_ssl) {
    std::string content_type;

    // Determine response type
    switch (this->type) {
        case HTML:
            content_type = "text/html";
            break;
        case CSS:
            content_type = "text/css";
            break;
        case PNG:
            content_type = "image/png";
            break;
        case JS:
            content_type = "application/javascript";
            break;
        case JSON:
            content_type = "application/json";
            break;
        default:
            content_type = "text/html";
            break;
    }

    SSL_write(c_ssl,
            ("HTTP/1.1 " + errors_msg[this->status_code] + "\r\nContent-Type: " +  content_type + "\r\n").c_str(),
            27 + errors_msg[this->status_code].length() + content_type.length());

    // Add cookies to response
    if(this->type == HTML) {
        for(auto cookie : this->cookies) {
            SSL_write(c_ssl,
                    ("Set-Cookie: " + cookie.name + "=" +  cookie.value + "\r\n").c_str(),
                    15  + cookie.name.length() + cookie.value.length());
        }
    }

    // Dual endline to indicate start of HTTP response payload
    if(this->binary_file) {
        SSL_write(c_ssl, ("Content-Transfer-Encoding: binary\r\n\r\n"), 37);
        SSL_write(c_ssl, &this->binary[0], this->binary.size());
    }else if(this->type != HTML) {
        SSL_write(c_ssl, ("\r\n" + this->body.str()).c_str(), 2 + this->body.str().length());
    }else{
        SSL_write(c_ssl, ("\r\n" +
                    this->header +  this->body.str() + this->footer).c_str(),
                2 + this->header.length() + this->body.str().length() + this->footer.length());
    }
}

enum filetype page::get_filetype() {
    return this->type;
}


void page::set_status_code(enum errors status_code) {
    this->status_code = status_code;
}

enum errors page::get_status_code() {
    return this->status_code;
}


void page::set_filetype(enum filetype type) {
    this->type = type;
}

void page::response(struct networking::client_stream client) {
    if(client.ssl) {
        this->print_page_ssl(client.https);
    }else {
        this->print_page(client.tcp);
    }
}
