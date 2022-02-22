//
// Created by Shubham Sawant on 16/12/21.
//

#include <cstring>
#include <string>

#include "message.h"

Message::Message(size_t buffer_capacity) {
    reset_buffer(buffer_capacity);
}

Message::Message(const std::string message_data) {
    reset_buffer(message_data.size());
    set_data(message_data);
}

Message::~Message() {
     free(data_);
}

char* Message::data() const {
    return data_;
}

void Message::put_data(unsigned int index, char c) {
    if (index < buffer_capacity_) {
        data_[index] = c;
    }
    if (length_ < index) {
        length_ = index + 1;
    }
}

const std::string Message::data_str() const {
    if(data_) {
        return std::string(data_, length_);
    }
    return "";
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
    size_t data_size = data.size();
    if (buffer_capacity_ < data_size) {
        reset_buffer(data_size);
    }
    length_ = data_size;
    char* iter = data_;
    for(auto c : data) {
        *iter = c;
        iter++;
    }
}

Message::Message(Message&& message) {
    this->length_ = message.length_;
    this->buffer_capacity_ = message.buffer_capacity_;
    this->data_ = message.data_;
    message.data_ = nullptr;
}

void Message::reset_buffer(size_t buffer_capacity) {
    free(data_);
    length_ = 0;
    buffer_capacity_ = buffer_capacity;
    data_ = (char*)malloc(buffer_capacity_*sizeof(char));
    memset(data_,0,buffer_capacity_*sizeof(char));
}