//
// Created by Shubham Sawant on 30/12/21.
//

#include <iostream>
#include <memory>

#include "tcp_client/tcp_client.h"

const unsigned int PORT = 8081;

int main() {
    auto client = std::make_shared<TCPClient>("127.0.0.1", PORT, "first");
    if (client->SendMessage("Nice test")) {
        auto message = client->ReceiveMessage(512);
        PRINT_THREAD_ID
        std::cout << "Got : " << message->data() << "\n";
    }
    return 0;
}