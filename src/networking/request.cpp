 //[ Adam ]
 
// NOTE: Read the first comment inside request.hpp
#include "request.hpp"

using request_handler::request;
using request_handler::type;

enum type request_handler::str_to_type(std::string type) {

    if(type == "POST")
        return request_handler::POST;
    else if(type == "GET")
        return request_handler::GET;

    return request_handler::ERROR_TYPE;
}

bool request_handler::key_exists(const char *key, 
        std::unordered_map<std::string, std::string> &hashmap) {

    return (hashmap.find(key) != hashmap.end());
}


/*  The constructor for the request class interprets
 *  a HTTP request.
 *
 * GET request looks like this:
 * GET /path/here HTTP/x.x
 * Host: cloudpotato.xyz
 * Cookie: name=value; other_name=other_value
 *
 * POST request looks like this:
 * POST /path/here HTTP/x.x
 * Host: cloudpotato.xyz
 * Cookie: name=value; other_name=other_value
 * Content-Type: application/x-www-form-urlencoded
 * Content-Length: 27
 *
 * field1=value1&field2=value2
 *
 */
request::request(char *http_request) {

    // The char ptr points to an allocated buffer
    std::string_view req(http_request);

    // # Parsing first line of HTTP request, each field is separated
    // with whitespace

    // Parse first GET/POST token
    int next_pos = req.find_first_of(' ');

    if(req.substr(0, next_pos) == "GET") {
        this->request_type = GET;
    }else if(req.substr(0, next_pos) == "POST") {
        this->request_type = POST;
    }else {
        this->request_type = ERROR_TYPE;
    }

    // Parse URL that comes after GET/POST
    req = req.substr(next_pos + 1);
    next_pos = req.find_first_of(' ');
    this->url = req.substr(0, next_pos);

    // Ignore the remaining HTTP/x.x 
    next_pos = req.find_first_of('\n');

    // Setup to iterate over each line in the HTTP request
    req = req.substr(next_pos + 1);
    std::string_view line = req.substr(0, req.find_first_of('\n') - 1);

    while(!req.empty()) {

        // The final line always contains a new line at index 0
        // POST data when in x-www-form-urlenconded encoding will
        // come after this line
        if( (line[0] == '\n' || line[0] == '\r') ) {
            req = req.substr(req.find_first_of('\n') + 1);
            line = req.substr(0, req.find_first_of('\n'));

            if(this->request_type == POST)
                this->get_post(std::string(line));
            break; 
        }

        // Check if the HTTP request field contains
        // any values we're interested in. E.g. cookies
        this->parse_request(line);

        // Fetch next line
        req = req.substr(req.find_first_of('\n') + 1);
        line = req.substr(0, req.find_first_of('\n') - 1);
    }
} 

// Extract post data (enconded as x-www-form-urlenconded)
// Example POST data:
// username=john&password=Jf25F_w==/2@sd!98eVa
//
// Default encoding for HTTP form with method set to POST
void request::get_post(std::string post_raw) {

    std::stringstream post_stream(post_raw);
    char delimiter = '&';

    // Splitting tokens by &
    for(std::string token; 
        std::getline(post_stream, token, delimiter);) {

        struct request_handler::post new_post;

        // Parsing cookie which looks like this:
        // name_here=value_here
        int equal_sign_pos = token.find('=');
        new_post.name = token.substr(0, equal_sign_pos);

        token.erase(0, equal_sign_pos+1);

        // Inserting POST into hashmap
        new_post.value = token;
        this->posts[new_post.name] = new_post.value;

    }

}

// Parses HTTP request field, e.g.:
// Cookie: username=admin; session=835235-addsf-352-dsafds
void request::parse_request(std::string_view request) {

    int pos = request.find(':');
    std::string_view token = request.substr(0, pos + 1);

    if(token == "Cookie:") {
        cookie_handler::get_cookies(this->cookies, std::string(request.substr(pos + 1))) ;
    }else if(token == "Accept-Encoding:") {

    }
}

