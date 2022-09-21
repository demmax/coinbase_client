//
// Created by max on 21.09.2022.
//


#include <boost/asio/ip/udp.hpp>
#include "../tick.pb.h"

int main()
{
    boost::asio::io_context io_context;
    boost::asio::ip::udp::socket socket{io_context};
    boost::asio::ip::udp::endpoint ep{boost::asio::ip::address::from_string("127.0.0.1"), 55556};
    socket.open(boost::asio::ip::udp::v4());
    socket.bind(ep);

    std::cout << "socket bound" << std::endl;

    while (1) {
        std::vector<char> vec(1024);
//        auto read = socket.receive(boost::asio::buffer(vec));
//        int size = vec[0];
//        std::cout << "datacount received" << vec.size() << std::endl;
//        std::cout << "size received" << size << std::endl;
//        std::cout << "read" << read << std::endl;
//        int tick_size = size;
        std::vector<char> tick_data(1024);

            auto bytes = socket.receive(boost::asio::buffer(tick_data));

        int size = tick_data[0];
            std::cout << "data received" << bytes << std::endl;
            std::cout << "tick data" << tick_data.size() << std::endl;
            std::cout << "data[0]" << (uint32_t)tick_data[0] << std::endl;
//            std::copy (vec.begin(), vec.end(), std::back_inserter(tick_data));
//            size -= bytes;

        std::cout << "parsing" << std::endl;
        Tick tick;
        std::cout << tick_data.size() << std::endl;
        tick.ParseFromArray(tick_data.data() + 1, size);
        std::cout << tick.DebugString() << std::endl;
        std::cout << tick.ByteSizeLong() << std::endl;
        std::cout << tick.best_ask() << std::endl;
//        std::copy(tick_data.begin(), tick_data.end(), std::ostream_iterator<uint8_t>(std::cout, " "));
        std::cout << std::endl;
    }
}