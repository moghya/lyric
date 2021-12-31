//
// Created by Shubham Sawant on 30/12/21.
//

#include <memory>

#include "tcp_server_app_client.h"

TCPServerAppClient::TCPServerAppClient(std::string ip_address,
                                       unsigned int port,
                                       std::string  name) {
    tcp_client_ = std::make_shared<TCPClient>(ip_address, port, name);
}

TCPServerAppClient::~TCPServerAppClient() {}