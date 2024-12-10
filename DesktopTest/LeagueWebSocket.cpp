#include "LeagueWebSocket.h"

#include <iostream>
#include "base64.h"


context_ptr LeagueWebSocket::on_tls_init() {
    context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::no_sslv3 |
            boost::asio::ssl::context::single_dh_use);
    }
    catch (std::exception& e) {
        std::cout << "Error in context pointer: " << e.what() << std::endl;
    }
    return ctx;
}

void LeagueWebSocket::run() {
    try {
        // Initialize ASIO
        endpoint.init_asio();

        // Set logging to be pretty verbose
        endpoint.set_access_channels(websocketpp::log::alevel::all);
        endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);

        endpoint.set_tls_init_handler(bind(&on_tls_init));

        // Bind handlers
        endpoint.set_open_handler([this](websocketpp::connection_hdl hdl) { on_open(hdl); });
        endpoint.set_message_handler([this](websocketpp::connection_hdl hdl, client::message_ptr msg) { on_message(hdl, msg); });
        endpoint.set_close_handler([this](websocketpp::connection_hdl hdl) { on_close(hdl); });

        // Create a connection to the given URL
        websocketpp::lib::error_code ec;
        client::connection_ptr con = endpoint.get_connection("wss://127.0.0.1:" + port, ec);
        if (ec) {
            std::cerr << "Error: " << ec.message() << std::endl;
            return;
        }

        // Add authentication header
        std::string auth_header = "Authorization: Basic " + base64_encode("riot:x" + token);
        con->append_header("Authorization", auth_header);

        // Connect
        endpoint.connect(con);

        // Start the ASIO event loop
        endpoint.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception" << std::endl;
    }
}
void LeagueWebSocket::on_open(websocketpp::connection_hdl hdl) {
    std::cout << "Connected to League WebSocket!" << std::endl;

    // Example: Subscribe to an event
    std::string subscribe_message = R"(["subscribe", "OnJsonApiEvent_lol-champ-select_v1_session"])";
    endpoint.send(hdl, subscribe_message, websocketpp::frame::opcode::text);
}

void LeagueWebSocket::on_message(websocketpp::connection_hdl, client::message_ptr msg) {
    std::cout << "Message received: " << msg->get_payload() << std::endl;
}

void LeagueWebSocket::on_close(websocketpp::connection_hdl) {
    std::cout << "WebSocket connection closed." << std::endl;
}
