//
// Created by Shubham Sawant on 16/12/21.
//


#include <memory>
#include <string>

#include "tcp_connection.h"
#include "tcp_utils.h"



TCPConnection::TCPConnection() {
    address_ = (struct sockaddr*) malloc(sizeof(struct sockaddr_in));
    address_length_ = (socklen_t*) malloc(sizeof(socklen_t));
}


TCPConnection::TCPConnection(struct sockaddr* address,
                             unsigned int socket_fd)
    : address_(address),
      socket_fd_(socket_fd) {
    address_length_ = (socklen_t*) malloc(sizeof(socklen_t));
    *address_length_ = sizeof (address_);
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

std::unique_ptr<Message> TCPConnection::ReceiveMessage(size_t buffer_capacity) {
    auto message = std::move(tcp_util::receive_stream_message(
                            this->socket_fd(),buffer_capacity));
    message->put_data(message->length(), 0);
    return std::move(message);
}

bool TCPConnection::SendMessage(std::string message_data) const {
    auto message = std::make_unique<Message>(message_data);
    size_t sent_size = tcp_util::send_stream_message(
                this->socket_fd(), std::move(message));
    return sent_size == message_data.size();
}