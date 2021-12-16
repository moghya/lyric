//
// Created by Shubham Sawant on 16/12/21.
//


#include <memory>
#include <string>

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

std::shared_ptr<Message> TCPConnection::ReceiveMessage(size_t buffer_capacity) {
    std::shared_ptr<Message> message;
    message = std::make_unique<Message>(buffer_capacity);
    auto message_length = recvfrom(this->socket_fd(),
                                   (void*) message->data(),
                                   message->buffer_capacity(),
                                   0,
                                   this->address(),
                                   this->address_length_ptr());
    message->set_length(message_length);
    message->put_data(message_length, 0);
    return message_length == 0 ? nullptr : message;
}

bool TCPConnection::SendMessage(std::string message_data) {
    std::shared_ptr<Message> message;
    message = std::make_unique<Message>(message_data);
    size_t sent_size = sendto(this->socket_fd(),
                              (void*) message->data(),
                              message->length(),
                              0,
                              this->address(),
                              this->address_length());
    return sent_size == message_data.size();
}