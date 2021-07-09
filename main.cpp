#include <iostream>

#include "key_value_store_app/key_value_store_app.h"
#include "tcp_server/tcp_server.h"


const unsigned int PORT = 8080;
const unsigned int STORE_CAPACITY = 1000;

int main() {
    std::shared_ptr<KeyValueStoreApp> app = std::make_shared<KeyValueStoreApp>(
      "my-key-value-store",
      STORE_CAPACITY,
      KeyValueStoreApp::EvictionPolicy::kFIFO);
    auto server = std::make_shared<TCPServer>(app, PORT);
    server->StartListening();
    return 0;
}
