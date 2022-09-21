#!/usr/bin/env python3
import socket
import sys

import tick_pb2


if __name__ == "__main__":
    assert len(sys.argv) == 2
    host, port = sys.argv[1].split(":")
    port = int(port)

    print(f"Start listening on host {host} and port {port}")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((host, port))

    tick_data = bytes()
    while True:
        buf = sock.recv(1024)
        tick_data += buf
        while tick_data:
            pkg_size = tick_data[0]
            if pkg_size > len(tick_data):
                break

            tick = tick_pb2.Tick()
            tick.ParseFromString(tick_data[1:pkg_size + 1])
            tick_data = tick_data[pkg_size + 1:]
            print("Consumer received message:\n%s" % tick)
