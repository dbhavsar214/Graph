#include ".h"
#include <libwebsockets.h>
#include <nlohmann/json.hpp> 
#include <iostream>
#include <cstring>
#include <csignal>

static volatile bool interrupted = false;
ArbitrageEngine Ae(9);

// Signal handler
static void signal_handler(int sig) {
    interrupted = true;
}

// Static callback function
static int callback_client(struct lws* wsi, enum lws_callback_reasons reason,
    void* user, void* in, size_t len) {
    switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED: {
        std::cout << "Client connected to server!" << std::endl;
        break;
    }

    case LWS_CALLBACK_CLIENT_RECEIVE: {
        std::string msg((const char*)in, len);

        try {
            // Parse JSON from message
            auto j = nlohmann::json::parse(msg);

            std::string exch = j.at("exchange").get<std::string>();
            double price = j.at("p").get<double>();

            Quote qt{ price, exch };
            Ae.receiveQuote(exch, qt);

        }
        catch (const std::exception& e) {
            std::cerr << "JSON parse error or missing field: " << e.what() << "\n";
        }

        break;
    }

    case LWS_CALLBACK_CLIENT_CLOSED: {
        std::cout << "Connection closed by server." << std::endl;
        interrupted = true;
        break;
    }

    default:
        break;
    }
    return 0;
}

// Define protocols
static struct lws_protocols protocols[] = {
    {
        "ws-client-protocol", callback_client, 0, 4096,
    },
    { nullptr, nullptr, 0, 0 }
};

WSConnection::WSConnection()
    :context_(nullptr), wsi_(nullptr) {
}

bool WSConnection::setConnection(int port) {
    signal(SIGINT, signal_handler); // Handle Ctrl+C

    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, nullptr);

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    context_ = lws_create_context(&info);
    if (!context_) {
        std::cerr << "Failed to create WebSocket context\n";
        return false;
    }

    struct lws_client_connect_info ccinfo = {};
    ccinfo.context = context_;
    ccinfo.address = "localhost";             // Set your actual server address
    ccinfo.port = port;                       // Port number
    ccinfo.path = "/";
    ccinfo.host = lws_canonical_hostname(context_);
    ccinfo.origin = "origin";
    ccinfo.protocol = nullptr;
    ccinfo.ssl_connection = 0;

    wsi_ = lws_client_connect_via_info(&ccinfo);
    if (!wsi_) {
        std::cerr << "Failed to connect to server\n";
        lws_context_destroy(context_);
        return false;
    }

    // Run the event loop
    while (!interrupted) {
        lws_service(context_, 100);
    }

    lws_context_destroy(context_);
    std::cout << "Exiting client cleanly.\n";
    return true;
}