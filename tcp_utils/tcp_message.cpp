//
// Created by Shubham Sawant on 16/12/21.
//

#include <string>

#include "tcp_message.h"

TCPMessage::TCPMessage(size_t buffer_capacity,
                       std::shared_ptr<TCPConnection> sender,
                       std::shared_ptr<TCPConnection> receiver) :
        sender_(sender),
        receiver_(receiver),
        buffer_capacity_(buffer_capacity),
        length_(buffer_capacity_) {
    data_ = new char[buffer_capacity_];
}

TCPMessage::~TCPMessage() {
    delete data_;
}

const std::shared_ptr<TCPConnection>& TCPMessage::sender() const {
    return sender_;
}

char* TCPMessage::data() const {
    return data_;
}

void TCPMessage::put_data(unsigned int index, char c) {
    if (index < buffer_capacity_) {
        data_[index] = c;
    }
}

const std::string TCPMessage::data_str() const {
    return std::string(data_);
}

void TCPMessage::set_length(size_t length) {
    length_ = length;
}

size_t TCPMessage::length() const {
    return length_;
}

size_t TCPMessage::buffer_capacity() const {
    return buffer_capacity_;
}

void TCPMessage::set_data(std::string data) {
    strcpy(data_, data.c_str());
    length_ = data.length();
}

const std::shared_ptr<TCPConnection>& TCPMessage::receiver() const {
    return receiver_;
}


void TCPMessage::AppendNewLine() {
    this->put_data(this->length(),'\n');
    this->set_length(this->length()+1);
}