FROM gcc:11 as coinbase_client

# gcc image has too old cmake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.22.5/cmake-3.22.5-Linux-x86_64.sh \
      -q -O /tmp/cmake-install.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/cmake-install.sh

RUN apt-get update && \
    apt-get upgrade && \
    apt-get install -y python3 && \
    apt-get install -y libprotobuf-dev && \
    apt-get install -y python3-pip && \
    apt-get install -y protobuf-compiler && \
    pip3 install protobuf==3.20.0 && \
    pip3 install conan

ENV PATH="/usr/bin/cmake/bin:${PATH}"

ADD start.sh conanfile.txt CMakeLists.txt tick.proto /
ADD ./src /src
ADD ./consumer /consumer

WORKDIR /build
RUN conan install .. --remote conancenter --build missing --settings build_type=Release --settings compiler.libcxx=libstdc++11
RUN protoc -I=/ --cpp_out=/src/ --python_out /consumer/ /tick.proto
RUN cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . -j8

RUN chmod +x /start.sh && chmod +x /consumer/consumer.py
ENTRYPOINT ["/start.sh"]
