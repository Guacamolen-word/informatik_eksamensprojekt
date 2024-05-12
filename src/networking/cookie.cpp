
#include "cookie.hpp"


// NOTE: Unused
enum errors cookie_handler::get_cookies_vector(std::vector<std::unique_ptr<cookie>> &out_cookies) {

    char *cookie_check = std::getenv("HTTP_COOKIE");
    std::string cookies;

    if(cookie_check == NULL)
        return NO_COOKIES;
    else
        cookies = std::string(cookie_check);

    std::stringstream cookie_stream(cookies);
    char delimiter = ';';


    for(std::string token;
        std::getline(cookie_stream, token, delimiter);) {

        std::unique_ptr<struct cookie_handler::cookie> new_cookie ( new struct cookie_handler::cookie() );

        // Parsing cookie which looks like this:
        // name_here=value_here
        int equal_sign_pos = token.find('=');
        new_cookie->name = token.substr(0, equal_sign_pos);

        token.erase(0, equal_sign_pos + 1);

        new_cookie->value = token;
        out_cookies.push_back(std::move(new_cookie));
    }
    

    return OK;
}

// Parses cookies from from HTTP request Cookie field and stores them in hashmap
// Cookied are formatted like so:
// session=awa-242-asd; username=john
enum errors cookie_handler::get_cookies(std::unordered_map< std::string, std::string > &out_cookies, std::string cookies) {

    std::stringstream cookie_stream(cookies);
    char delimiter = ';';

    // Iterate through each cookie by using ; as a delimiter
    for(std::string token;
        std::getline(cookie_stream, token, delimiter);) {

        std::unique_ptr<struct cookie_handler::cookie> new_cookie ( new struct cookie_handler::cookie() );

        // Parsing token which looks like this:
        // name_here=value_here
        int equal_sign_pos = token.find('=');
        new_cookie->name = token.substr(0, equal_sign_pos);

        while(new_cookie->name[0] == ' ')
            new_cookie->name.erase(0, 1);

        token.erase(0, equal_sign_pos + 1);

        new_cookie->value = token;
        
        // Store result in hashmap
        out_cookies[new_cookie->name] = new_cookie->value;
    }
    

    return OK;
}
