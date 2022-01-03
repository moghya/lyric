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


TCPClient::TCPClient(std::string dest_ip_address,
                     unsigned int dest_port,
                     std::string name) :
    dest_ip_address_(dest_ip_address),
    dest_port_(dest_port),
    name_(name) {
    /* Change this to get TCPAddress and TCPSocket for TCPConnection Constructor */
    struct addrinfo hints;
    struct addrinfo* res;
    int sockfd;
    // first, load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(dest_ip_address_.c_str(), std::to_string(dest_port_).c_str(), &hints, &res);
    // make a socket:
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        std::cout << "did not get sockfd";
    }
    if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0 ) {
        std::cout << "could not connect";
    }
    connection_ = std::make_shared<TCPConnection>(res->ai_addr, sockfd);
}

TCPClient::~TCPClient() {}
