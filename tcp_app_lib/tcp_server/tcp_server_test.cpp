//
// Created by Shubham Sawant on 30/12/21.
//

#include <iostream>
#include <memory>

#include "../../third_party/spdlog/include/spdlog/spdlog.h"

#include "tcp_server.h"

const unsigned int PORT = 8081;

int main() {
    spdlog::set_pattern("[%l] [%n] [%A-%d-%m-%Y] [%H:%M:%S] [%z] [%t] %s:%# %v");
    TCPMessage::Handler handler = [](
        std::shared_ptr<TCPMessage> tcp_message) -> tcp_util::ACTION_ON_CONNECTION {
        SPDLOG_INFO(fmt::format("Received message: {} from: {}",
                                tcp_message->message()->data(),
                                tcp_message->sender()->socket_fd()));
        tcp_message->sender()->SendMessage("Thanks for message, closing your connection.");
        return tcp_util::ACTION_ON_CONNECTION::CLOSE;
    };
    auto server = std::make_shared<TCPServer>(PORT, handler);
    server->StartListening();

    return 0;
}