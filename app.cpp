#include <arpa/inet.h>
#include <iostream>
#include <memory>

#include "third_party/spdlog/include/spdlog/spdlog.h"
#include "key_value_store_app/key_value_store_app.h"

const unsigned int PORT = 8081;
const unsigned int STORE_CAPACITY = 1000;

int main() {
    spdlog::set_pattern("[%l] [%n] [%A-%d-%m-%Y] [%H:%M:%S] [%z] [%t] %s:%# %v");
    auto app = std::make_unique<KeyValueStoreApp>(
            "my-key-value-store", STORE_CAPACITY,
            KeyValueStore::EvictionPolicy::kFIFO, PORT);
    app->Start();
    return 0;
}