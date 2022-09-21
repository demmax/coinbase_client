#include <iostream>

#include "coinbase_client.h"
#include "udp_publisher.h"
#include "async_message_converter.h"


std::tuple<std::string, uint16_t> parse_endpoint(const std::string& endpoint)
{
    std::size_t idx = endpoint.find(':');
    if (idx == std::string::npos) {
        return {};
    }

    std::string ip = endpoint.substr(0, idx);
    std::string port_str = endpoint.substr(idx + 1);
    int int_port = std::stoi(port_str);
    auto port = (uint16_t)int_port;
    if (port != int_port) {
        return {};
    }

    return std::make_tuple(ip, port);
}

int main(int argc, char** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc < 3) {
        std::cout << "Usage: coinbase_client <host>:<port> <symbol>" << std::endl;
        return 1;
    }

    std::string full_endpoint = argv[1];
    std::string symbol = argv[2];
    auto [host, port] = parse_endpoint(full_endpoint);
    if (host.empty() || !port) {
        std::cout << "Usage: coinbase_client <host>:<port> <symbol>" << std::endl;
        return 1;
    }

    std::cout << "symbol: " << symbol << std::endl;
    std::cout << "host: " << host << std::endl;
    std::cout << "port: " << port << std::endl;

    UdpPublisher udp_publisher(host, port);
    AsyncMessageConverter converter([&udp_publisher] (std::shared_ptr<std::vector<char>> data) {
        udp_publisher.sendData(std::move(data));
    });

    CoinbaseClient client("wss://ws-feed.exchange.coinbase.com",
                          symbol,
                          [&converter] (WebSocketMsgPtr msg_ptr) {
                                converter.enqueue_message(std::move(msg_ptr));
                          });

    std::thread udp_thread([&udp_publisher]() { udp_publisher.run(); });
    std::thread converter_thread([&converter]() { converter.run(); });
    std::thread ws_thread([&client]() { client.run(); });

    ws_thread.join();
    converter_thread.join();
    udp_thread.join();

    return 0;
}
