//
// Created by Shubham Sawant on 19/12/21.
//

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

#include "../../third_party/spdlog/include/spdlog/spdlog.h"
#include "../tcp_utils/tcp_socket.h"

#include "tcp_client.h"


TCPClient::TCPClient(std::string dest_ip_address,
                     unsigned int dest_port,
                     std::string name) :
                                         dest_ip_address_(dest_ip_address),
                                         dest_port_(dest_port),
                                         name_(name) {
    connection_ = std::make_shared<TCPConnection>(dest_ip_address_, dest_port_);
}

TCPClient::~TCPClient() {
}
