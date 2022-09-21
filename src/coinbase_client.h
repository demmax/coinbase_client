//
// Created by max on 20.09.2022.
//

#ifndef COINBASE_CLIENT_COINBASE_CLIENT_H
#define COINBASE_CLIENT_COINBASE_CLIENT_H


#include <string>
#include <boost/json/object.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include "common_types.h"


class CoinbaseClient
{
public:
    using TickHandler = std::function<void(WebSocketMsgPtr)>;

    CoinbaseClient(std::string endpoint, std::string symbol, TickHandler handler);

    CoinbaseClient(const CoinbaseClient& o) = delete;
    const CoinbaseClient& operator=(const CoinbaseClient& o) = delete;

    void run();

private:
    using WSSClient = websocketpp::client<websocketpp::config::asio_tls_client>;
    using ContextPtr = std::shared_ptr<boost::asio::ssl::context>;

    void on_message(WSSClient* c,
                    websocketpp::connection_hdl hdl,
                    websocketpp::config::asio_client::message_type::ptr msg);

    void on_open(WSSClient* c, websocketpp::connection_hdl hdl);
    static ContextPtr on_tls_init();

    WSSClient m_client;
    std::string m_endpoint;
    std::string m_symbol;
    TickHandler m_tick_handler;
};


#endif //COINBASE_CLIENT_COINBASE_CLIENT_H
