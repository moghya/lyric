//
// Created by Shubham Sawant on 16/12/21.
//


#include <memory>

#include "tcp_connection.h"



TCPConnection::TCPConnection() {
    address_ = (struct sockaddr*) malloc(sizeof(struct sockaddr_in));
    address_length_ = (socklen_t*) malloc(sizeof(socklen_t));
}

TCPConnection::~TCPConnection() {
    free(address_);
    free(address_length_);
}

struct sockaddr* TCPConnection::address() const {
    return address_;
}

socklen_t* TCPConnection::address_length_ptr() const {
    return address_length_;
}

size_t TCPConnection::address_length() const {
    return sizeof(address_);
}

void TCPConnection::set_socket_fd(unsigned int sokect_fd) {
    socket_fd_ = sokect_fd;
}

unsigned int TCPConnection::socket_fd() const {
    return socket_fd_;
}