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
- results: run this cmd to benchmark the server `wrk -t10 -c500 -d60s http://0.0.0.0:8080/ `
    ``` cmd
    10 threads and 500 connections
    Thread Stats   Avg      Stdev     Max   +/- Stdev
        Latency     8.92ms    9.07ms 472.84ms   99.69%
        Req/Sec     5.83k   260.17     6.89k    94.80%
    3480746 requests in 1.00m, 338.59MB read
    Requests/sec:  57995.56
    Transfer/sec:      5.64MB
    ```
    As we can see, server is able to handle more than 30k requests per second


credits [repo](www.google.com)
