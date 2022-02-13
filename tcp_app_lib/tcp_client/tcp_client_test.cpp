//
// Created by Shubham Sawant on 30/12/21.
//

#include <iostream>
#include <memory>

#include "../../third_party/spdlog/include/spdlog/spdlog.h"
#include "tcp_client.h"

const unsigned int PORT = 8081;

int main() {
    spdlog::set_pattern("[%l] [%n] [%A-%d-%m-%Y] [%H:%M:%S] [%z] [%t] %s:%# %v");
    auto client = std::make_shared<TCPClient>("127.0.0.1", PORT, "first");
    if (client) {
        auto send_res = client->SendMessage("Nice test");
        if (send_res.success_) {
            auto recv_res = client->ReceiveMessage(512);
            SPDLOG_INFO(recv_res.result_->data_str());
        }
    }
    return 0;
}