#pragma once

#include "cookie.hpp"
#include "page.hpp"
#include "request.hpp"
#include "db.hpp"
#include "server.hpp"
#include "../json/json.hpp"

using response_handler::read_file;
using response_handler::page;
using response_handler::filetype::HTML;
using response_handler::filetype::CSS;
using response_handler::filetype::JS;
using response_handler::filetype::JSON;

using request_handler::GET;
using request_handler::POST;

