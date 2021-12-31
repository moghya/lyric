//
// Created by Shubham Sawant on 16/12/21.
//

#include <string>

#include "message.h"

Message::Message(size_t buffer_capacity) :
        buffer_capacity_(buffer_capacity),
        length_(buffer_capacity_) {
    data_ = new char[buffer_capacity_];
}

Message::Message(const std::string& message_data) :
        buffer_capacity_(message_data.size()) {
    data_ = new char[buffer_capacity_];
    set_data(message_data);
}


Message::~Message() {
    delete data_;
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

void Message::set_data(const std::string& data) {
    if (buffer_capacity_ < data.length()) {
        delete data_;
        buffer_capacity_ = data.length();
        data_ = new char[buffer_capacity_];
    }
    length_ = data.length();
    stpncpy(data_, data.c_str(), length_);
}

void Message::AppendNewLine() {
    this->put_data(this->length(),'\n');
    this->set_length(this->length()+1);
}

Message::Message(Message&& message) {
    this->length_ = message.length_;
    this->buffer_capacity_ = message.buffer_capacity_;
    this->data_ = message.data_;
    message.data_ = nullptr;
}
