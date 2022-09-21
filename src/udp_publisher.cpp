//
// Created by max on 20.09.2022.
//

#include <utility>
#include <functional>

#include <boost/bind/bind.hpp>

#include "udp_publisher.h"


using boost::asio::ip::address;
using boost::asio::ip::udp;


UdpPublisher::UdpPublisher(const std::string& address, uint16_t port) :
    m_io_context(1),
    m_socket(m_io_context),
    m_endpoint(address::from_string(address), port)
{
}

void UdpPublisher::run()
{
    m_socket.open(udp::v4());
    m_socket.connect(m_endpoint);
    m_io_context.run();
}

void UdpPublisher::sendData(std::shared_ptr<std::vector<char>> data)
{
    m_socket.async_send(boost::asio::buffer(*data),
                        boost::bind(&UdpPublisher::write_callback,
                                    this,
                                    data,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
}

void UdpPublisher::write_callback(
        std::shared_ptr<std::vector<char>> data,
        const boost::system::error_code &ec,
        std::size_t bytes_transferred)
{
    if (ec) {
        std::cerr << "Error while sending through UDP: " << ec.message() << std::endl;
        return;
    }

    // Not so effective, but I don't expect this case too much.
    if (bytes_transferred < data->size()) {
        data->erase(data->begin(), data->begin() + bytes_transferred);
        sendData(data);
    }
}
