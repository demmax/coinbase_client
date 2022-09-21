cmake_minimum_required(VERSION 3.18)
project(coinbase_client)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})

find_package(Boost REQUIRED COMPONENTS json)
find_package(OpenSSL REQUIRED)
find_package(websocketpp REQUIRED)
find_package(Protobuf REQUIRED)

set (SOURCES src/main.cpp
             src/coinbase_client.cpp
             src/udp_publisher.cpp
        src/async_message_converter.cpp
        )

add_executable(coinbase_client ${SOURCES})
target_include_directories(coinbase_client PRIVATE ${CONAN_INCLUDE_DIRS})
target_link_libraries(coinbase_client PRIVATE
        OpenSSL::SSL
        Boost::json
        protobuf::libprotobuf
)
