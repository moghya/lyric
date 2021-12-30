//
// Created by Shubham Sawant on 30/12/21.
//

#include <iostream>
#include <memory>

#include "tcp_client/tcp_client.h"

const unsigned int PORT = 8081;

int main() {
    auto client = std::make_shared<TCPClient>();
    client->AddConnection("127.0.0.1", PORT, "first");
    auto connection = client->GetConnection(0);
    if (connection->SendMessage("Nice test")) {
        auto message = connection->ReceiveMessage(512);
        PRINT_THREAD_ID
        std::cout << "Got : " << message->data() << "\n";
    }
    return 0;
}