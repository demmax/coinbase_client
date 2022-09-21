
### Quick start
```
git clone https://github.com/demmax/dns_cache/coinbase_client
cd coinbase_client
docker build -f Dockerfile . -t mdemianov/coinbase_client
docker run -e PYTHONUNBUFFERED=1 mdemianov/coinbase_client ETH-USD
```

For simplicity, coinbase client and consumer runs in the same image.


### Recap on design
Basically, application consists of 3 parts: 
 - `CoinbaseClient`
 - `AsyncMessageConverter`
 - `UdpPusblisher`


`CoinbaseClient` is responsible for connecting to websocket and receiving data. When data received, it  enqueues message to 
`AsyncMessageConverter`, which serializes message to `protobuf` in a separate thread, and sends it to `UdpPublisher`.

### Known issues
Sometimes `websocketpp` fails to connect to server with error 
```
[fail] WebSocket Connection Unknown - "" / 0 websocketpp.transport:9 Timer Expired
```
Seems to be a pretty common issue, but I didn't find any specific reason or 
feasible solution.
