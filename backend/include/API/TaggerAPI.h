/***********************************
 *  Name:           TaggerAPI.h
 *  Description:    Endpoints
 *                  Registers endpoints accessing FileTagger functionality
 *  Author(s):
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/

#ifndef PALETTE_TAGGERAPI_H
#define PALETTE_TAGGERAPI_H
#include "WhitelistMiddleware.h"
#include "crow/app.h"


inline void ExplorerAPI (crow::App<WhitelistMiddleware>& app) {
    // register endpoints here...
}



#endif //PALETTE_TAGGERAPI_H