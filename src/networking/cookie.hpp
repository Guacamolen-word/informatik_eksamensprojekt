#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "errors.hpp"

namespace cookie_handler {

    struct cookie{
        std::string name, value;
    };

    // NOTE: unused
    enum errors get_cookies_vector(std::vector< std::unique_ptr<cookie> >&);

    // Parses cookies from from HTTP request Cookie field and stores them in hashmap
    enum errors get_cookies(std::unordered_map< std::string, std::string >&, std::string);

};
