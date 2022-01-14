//
// Created by Shubham Sawant on 22/12/21.
//

#include <iostream>
#include <memory>

#include "key_value_store_app/key_value_store_app_client.h"

const unsigned int PORT = 8081;

int main() {
    auto client = std::make_shared<KeyValueStoreAppClient>("my_client", "127.0.0.1", PORT, 1);
    std::cout << client->GetEntry("hi") << "\n";
    std::cout << client->PutEntry("hi","hello") << "\n";
    std::cout << client->GetEntry("hi") << "\n";
    return 0;
}
