//
// Created by Shubham Sawant on 30/12/21.
//

#include <iostream>
#include <memory>

#include "tcp_server/tcp_server.h"

const unsigned int PORT = 8081;

int main() {
    TCPMessage::Handler handler = [](
        std::shared_ptr<TCPMessage> tcp_message) -> tcp_util::ACTION_ON_CONNECTION {
        PRINT_THREAD_ID
        std::cout << "Received message: " << tcp_message->message()->data()
                  << " from: " << tcp_message->sender()->socket_fd() << "\n";
        tcp_message->sender()->SendMessage("Thanks for message, closing your connectoin.");
        return tcp_util::ACTION_ON_CONNECTION::CLOSE;
    };
    auto server = std::make_shared<TCPServer>(PORT, handler);
    server->StartListening();

    return 0;
}