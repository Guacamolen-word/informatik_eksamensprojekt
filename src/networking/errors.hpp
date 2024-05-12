#pragma once

#include <string>

enum errors {OK = 0, UNAUTHORIZED, NOT_FOUND, IM_A_TEATPOT, INTERNAL_SERVER_ERROR,
    // Custom errors for internal error handling
    NO_COOKIES = 1000, DB_ERROR};

static std::string errors_msg[] {
    {"200 OK"},
    {"401 Unauthorized"},
    {"404 Not Found"},
    {"418 I'm a teapot"},
    {"500 Internal Server Error"},
};


