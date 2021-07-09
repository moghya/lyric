//
// Created by Shubham Sawant on 08/07/21.
//


#include "tcp_utils.h"


ClientConnection::ClientConnection() {
  address_ = (struct sockaddr*) malloc(sizeof(struct sockaddr_in));
  address_length_ = (socklen_t*) malloc(sizeof(socklen_t));
}

ClientConnection::~ClientConnection() {
  free(address_);
  free(address_length_);
}

struct sockaddr* ClientConnection::address() const {
  return address_;
}

socklen_t* ClientConnection::address_length_ptr() const {
  return address_length_;
}

size_t ClientConnection::address_length() const {
  return sizeof(address_);
}

void ClientConnection::set_socket_fd(unsigned int sokect_fd) {
  socket_fd_ = sokect_fd;
}

unsigned int ClientConnection::socket_fd() const {
  return socket_fd_;
}


Message::Message(std::shared_ptr<ClientConnection> client,
        size_t buffer_capacity) :
    client_(client),
    buffer_capacity_(buffer_capacity),
    length_(buffer_capacity_) {
  data_ = new char[buffer_capacity_];
}

Message::~Message() {
  delete data_;
}

std::shared_ptr<ClientConnection> Message::client() const {
  return client_;
}

char* Message::data() const {
  return data_;
}

void Message::put_data(unsigned int index, char c) {
  if (index < buffer_capacity_) {
    data_[index] = c;
  }
}

const std::string Message::data_str() const {
  return std::string(data_);
}

void Message::set_length(size_t length) {
  length_ = length;
}

size_t Message::length() const {
  return length_;
}

size_t Message::buffer_capacity() const {
  return buffer_capacity_;
}

void Message::set_data(std::string data) {
  strcpy(data_, data.c_str());
  length_ = data.length();
}

size_t ReceiveMessage(std::shared_ptr<Message> message) {
  auto message_length = recvfrom(message->client()->socket_fd(),
                                 (void*) message->data(),
                                 message->buffer_capacity(),
                                 0,
                                 message->client()->address(),
                                 message->client()->address_length_ptr());
  message->set_length(message_length);
  message->put_data(message_length, 0);
  return message_length;
}

size_t SendMessage(std::shared_ptr<Message> message, bool append_new_line) {
  if (append_new_line) {
    message->put_data(message->length(),'\n');
    message->set_length(message->length()+1);
  }
  return sendto(message->client()->socket_fd(),
                (void*) message->data(),
                message->length(),
                0,
                message->client()->address(),
                message->client()->address_length());
}