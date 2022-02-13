//
// Created by Shubham Sawant on 30/12/21.
//

#include <iostream>

#include "../../third_party/spdlog/include/spdlog/spdlog.h"

#include "..//tcp_client/tcp_client.h"
#include "tcp_server_app_client.h"

const unsigned int PORT = 8081;

int main() {
    spdlog::set_pattern("[%l] [%n] [%A-%d-%m-%Y] [%H:%M:%S] [%z] [%t] %s:%# %v");
    auto app_client = std::make_shared<TCPServerAppClient>("127.0.0.1", PORT, "client-1");
    auto send_res = app_client->tcp_client()->SendMessage("hi from app client");
    if (send_res.success_) {
        auto recv_res = app_client->tcp_client()->ReceiveMessage(512);
        if(recv_res.success_) {
            SPDLOG_INFO(fmt::format("Received message: {}", recv_res.result_->data()));
        }
    }
    return 0;
}