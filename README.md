# Npiregistry
api for npiregistry.cms.hhs.gov Search NPI Records
# main
```cpp
#include "Npiregistry.h"
#include <iostream>

int main() {
   Npiregistry api;
    auto records = api.search_npi_records(0,"washington").then([](json::value result) {
        std::cout << result<< std::endl;
    });
    records.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
