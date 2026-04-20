/***********************************
 *  Name:           WhitelistMiddleware.h
 *  Description:    Blocks all attempted connections to service that are not localhost
 *                  Currently, application only supports local connection, to allow remote proper auth needs implementation
 *  Author(s):      Xander Palermo <ajp2s@missouristate.edu
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/

#ifndef PALETTE_WHITELISTMIDDLEWARE_H
#define PALETTE_WHITELISTMIDDLEWARE_H
#include "crow/http_request.h"
#include "crow/http_response.h"

struct WhitelistMiddleware {
    struct context{};

    void before_handle(crow::request& request, crow::response& response, context& context) {
        if (const std::string& ip = request.remote_ip_address; ip != "127.0.0.1" && ip != "::1") { // If IP is foreign -> Deny request
            response.code = 403;
            response.end("Forbidden: Local Connection only");
        }
    }

    void after_handle(crow::request& request, crow::response& response, context& context) { /**Defualt**/ }
};


#endif //PALETTE_WHITELISTMIDDLEWARE_H