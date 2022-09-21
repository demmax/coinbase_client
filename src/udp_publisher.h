//
// Created by max on 20.09.2022.
//

#ifndef COINBASE_CLIENT_UDP_PUBLISHER_H
#define COINBASE_CLIENT_UDP_PUBLISHER_H

#include <string>

#include <boost/asio.hpp>

#include "tick.pb.h"


class UdpPublisher
{
public:
    UdpPublisher(const std::string& address, uint16_t port);

    void run();
    void sendData(std::shared_ptr<std::vector<char>> data);

protected:
    void write_callback(std::shared_ptr<std::vector<char>> data,
                        const boost::system::error_code& ec,
                        std::size_t bytes_transferred);

    boost::asio::io_context m_io_context;
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_endpoint;
};


#endif //COINBASE_CLIENT_UDP_PUBLISHER_H
