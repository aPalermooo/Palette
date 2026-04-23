/***********************************
 *  Name:           Controller.h
 *  Description:    API Handler
 *                  Listens on given port for user input from GUI/Tray Icon
 *  Author(s):      Xander Palermo <ajp2s@missouristate.edu
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/

#ifndef PALETTE_API_H
#define PALETTE_API_H
#include <thread>

#include "ExplorerAPI.h"
#include "TaggerAPI.h"
#include "WhitelistMiddleware.h"
#include "crow/app.h"

class Controller {
    crow::App<WhitelistMiddleware> app;
    std::thread thread;
    int port;

    /**
     * @brief handles queries of state and deactivation of process
     */
    void handleState () {

        CROW_ROUTE(app, "/")([] {
            /**
             * If palette background process is running, return code 200
             */
            return crow::response(200,  "Palette Online.");
        });

        CROW_ROUTE(app, "/exit")([this] {
            /**
             * Request to kill backend service
             */
            std::thread([this] {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                app.stop();
            }).detach();
            return crow::response(200, "Server shutdown complete.");
        });

    }

    /**
     * @brief Describes API end points object is listening to
     */
    void listen() {

        // Establish Endpoints
        handleState();
        ExplorerAPI(app);
        TaggerAPI(app);

        // Open port & begin listening
        app.port(port).run();
    }

public:
    explicit Controller(const int port) : port(port) {}

    ~Controller() {
        stop();
    }

    /**
     *
     * @brief Checks if API is running, if not, starts it
     *
     * @pre API thread is not yet running
     * @post API thread branches off and begins taking requests
     *
     */
    void start() {
        if (thread.joinable()) return;
        thread = std::thread(&Controller::listen, this);
    }

    /**
     *
     * @brief If API thread is running, join it back to main
     *
     * This method is called on destruction of the object
     *
     * @pre API is running
     * @post API stops and thread joins back to main
     *
     */
    void stop() {
        app.stop();
        if (thread.joinable()) {
            thread.join();
        }
    }
};


#endif //PALETTE_API_H