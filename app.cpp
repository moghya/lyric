#include <arpa/inet.h>
#include <iostream>
#include <memory>

#include "key_value_store_app/key_value_store_app.h"

const unsigned int PORT = 8081;
const unsigned int STORE_CAPACITY = 1000;

int main() {
    auto app = std::make_unique<KeyValueStoreApp>(
            "my-key-value-store", STORE_CAPACITY,
            KeyValueStoreApp::EvictionPolicy::kFIFO, PORT);
    app->Start();
    return 0;
}