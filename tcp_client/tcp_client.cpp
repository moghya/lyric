//
// Created by Shubham Sawant on 19/12/21.
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

#include "../tcp_utils/tcp_socket.h"
#include "tcp_client.h"


TCPClient::TCPClient() {
}

TCPClient::~TCPClient() {}

std::shared_ptr<const TCPConnection>
    TCPClient::AddConnection(std::string ip_address,
                             unsigned int port,
                             std::string name = "") {
    if (name == "") name = "connection_" + std::to_string(connections_.size());
    struct addrinfo hints;
    struct addrinfo* res;
    int sockfd;

    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(ip_address.c_str(), std::to_string(port).c_str(), &hints, &res);

    // make a socket:
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        std::cout << "did not get sockfd";
    }
    if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0 ) {
        std::cout << "could not connect";
    }
    auto connection = std::make_shared<TCPConnection>(res->ai_addr, sockfd);
    connections_.push_back(connection);
    connections_map_[name] = connections_.size() - 1;
    return connection;
}

std::shared_ptr<const TCPConnection> TCPClient::GetConnection(unsigned int index) const {
    if (index > connections_.size()) return nullptr;
    return connections_[index];
}

std::shared_ptr<const TCPConnection> TCPClient::GetConnectionByName(const std::string& name) const {
    return nullptr;
}