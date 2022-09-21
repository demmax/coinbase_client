//
// Created by max on 20.09.2022.
//

#include <utility>

#include <boost/json/serialize.hpp>

#include "coinbase_client.h"


CoinbaseClient::CoinbaseClient(std::string endpoint,
                               std::string symbol,
                               TickHandler handler) :
    m_endpoint(std::move(endpoint)),
    m_symbol(std::move(symbol)),
    m_tick_handler(std::move(handler))
{

}

void CoinbaseClient::on_message(WSSClient* c,
                                websocketpp::connection_hdl hdl,
                                websocketpp::config::asio_client::message_type::ptr msg)
{
    m_tick_handler(msg);
}

void CoinbaseClient::on_open(WSSClient* c, websocketpp::connection_hdl hdl)
{
    using namespace std::string_literals;

    std::cout << "Connection opened" << std::endl;

    boost::json::object obj;
    obj["type"] = "subscribe";
    obj["product_ids"] = {m_symbol};
    obj["channels"] = {"ticker"s};
    auto json_str = boost::json::serialize(obj);

    m_client.send(hdl, json_str, websocketpp::frame::opcode::text);
}

void CoinbaseClient::run()
{
    try {
        m_client.clear_access_channels(websocketpp::log::alevel::frame_header);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_client.set_error_channels(websocketpp::log::elevel::none);

        m_client.init_asio();

        // Register handlers
        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::placeholders::_2;
        m_client.set_tls_init_handler(bind(&CoinbaseClient::on_tls_init));
        m_client.set_open_handler(bind(&CoinbaseClient::on_open, this, &m_client, _1));
        m_client.set_message_handler(bind(&CoinbaseClient::on_message, this, &m_client, _1, _2));

        websocketpp::lib::error_code ec;
        WSSClient::connection_ptr con = m_client.get_connection(m_endpoint, ec);
        if (ec) {
            std::cout << ec.message() << std::endl;
            return;
        }

        m_client.connect(con);
        m_client.run();
    }
    catch (const std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    catch (const websocketpp::lib::error_code& e) {
        std::cerr << e.message() << std::endl;
    }
    catch (...) {
        std::cerr << "other exception" << std::endl;
    }
}

CoinbaseClient::ContextPtr CoinbaseClient::on_tls_init()
{
    auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);
    } catch (std::exception &e) {
        std::cerr << "Error in context pointer: " << e.what() << std::endl;
    }
    return ctx;
}
