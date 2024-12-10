#pragma once

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <string>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;


class LeagueWebSocket {
public:
	LeagueWebSocket(const std::pair<std::string, std::string> &clientDetails)
		: port(clientDetails.first), token(clientDetails.second) {
	}

	void run();
private:
	void on_open(websocketpp::connection_hdl hdl);
	void on_message(websocketpp::connection_hdl, client::message_ptr msg);
	void on_close(websocketpp::connection_hdl);
	static context_ptr on_tls_init();

	client endpoint;
	std::string port;
	std::string token;
};

