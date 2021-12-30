//
// Created by Shubham Sawant on 20/12/21.
//

#include "tcp_socket.h"
#include <sys/socket.h>

TCPSocket::TCPSocket(sa_family_t family) {
    fd_ = socket(family, SOCK_STREAM, 0);
}

TCPSocket::~TCPSocket() {}