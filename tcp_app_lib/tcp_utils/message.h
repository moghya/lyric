//
// Created by Shubham Sawant on 16/12/21.
//

#ifndef KEY_VALUE_STORE_MESSAGE_H
#define KEY_VALUE_STORE_MESSAGE_H

#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

/*
 * This class is a wrapper around message buffer filled using the data received
 * from client.
 * */
class Message {
public:
    Message(size_t buffer_capacity);
    Message(const std::string message_data);
    Message(Message&& message);
    ~Message();
    char* data() const;
    void put_data(unsigned int index, char c);
    void set_data(const std::string& data);
    const std::string data_str() const;
    void set_length(size_t length);
    size_t length() const;
    size_t buffer_capacity() const;
    void reset_buffer(size_t buffer_capacity);
private:
    size_t buffer_capacity_ = 0;
    size_t length_ = 0;
    char* data_ = nullptr;
};

#endif//KEY_VALUE_STORE_MESSAGE_H
