//
// Created by Shubham Sawant on 30/12/21.
//

#include <iostream>
#include "tcp_server_app_client.h"

const unsigned int PORT = 8081;

int main() {
    auto app_client = std::make_shared<TCPServerAppClient>("127.0.0.1", PORT, "client-1");
    if (app_client->tcp_client()->SendMessage("hi from app client")) {
        auto message = app_client->tcp_client()->ReceiveMessage(512);
        PRINT_THREAD_ID
        std::cout << "Got : " << message->data() << "\n";
    }
    return 0;
}