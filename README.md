## http_server_cpp

Http/1.0 server from scratch in c++ without using any third party library

#### SetupL
- clone the repo 
- `cd` into the repo `cd/http_server_cpp`
- run below command to build the project
    ```cmd
    make
    ```
- spin up the server 
    ```cmd
    ./build/http_server
    ```
    you should see `Server listening on 0.0.0.0:8080`
---

#### features:

---
#### file structure:

``` cmd
http-server/
├── build/              # Where compiled files go (created when you build)
│   ├── http_server     # The executable you run
│   ├── *.o             # Temporary object files
├── http_message.h      # Defines HTTP request/response structures
├── http_message.cpp    # Handles parsing requests and building responses
├── http_server.h       # Defines the server class
├── http_server.cpp     # Runs the server and processes requests
├── main.cpp            # Starts the server with example endpoints
└── Makefile            # Script to compile the project
```
---

#### Testing 
- I have used `wrk` to test my server 
- System details:
    ```cmd
        System Version: macOS 15.3.2 (24D81)
        Model Name: MacBook Air
        Model Identifier: MacBookAir10,1
        Model Number: MGN63HN/A
        Chip: Apple M1
        Total Number of Cores: 8 (4 performance and 4 efficiency)
        Memory: 8 GB
    ```
- results: run this cmd to benchmark the server `~ wrk -t4 -c100 -d10s http://localhost:8080/`
    ``` cmd
    Running 10s test @ http://localhost:8080/
    4 threads and 100 connections
    Thread Stats   Avg      Stdev     Max   +/- Stdev
        Latency   545.93us    5.13ms 173.84ms   99.46%
        Req/Sec     7.63k     3.47k   16.95k    67.94%
    299821 requests in 10.08s, 22.30MB read
    Socket errors: connect 0, read 307729, write 31, timeout 0
    Requests/sec:  29736.08
    Transfer/sec:      2.21MB

    ```
    As we can see, server is able to handle more than 30k requests per second


credits [repo](www.google.com)
