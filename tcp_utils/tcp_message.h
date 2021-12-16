//
// Created by Shubham Sawant on 16/12/21.
//

#ifndef KEY_VALUE_STORE_TCP_MESSAGE_H
#define KEY_VALUE_STORE_TCP_MESSAGE_H

#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tcp_connection.h"

class TCPConnection;

/*
 * This class is a wrapper around message buffer filled using the data received
 * from client.
 * */
class TCPMessage {
public:
    TCPMessage(size_t buffer_capacity,
               std::shared_ptr<TCPConnection> sender,
               std::shared_ptr<TCPConnection> receiver);
    ~TCPMessage();
    const std::shared_ptr<TCPConnection>& sender() const;
    const std::shared_ptr<TCPConnection>& receiver() const;
    char* data() const;
    void put_data(unsigned int index, char c);
    void set_data(std::string data);
    const std::string data_str() const;
    void set_length(size_t length);
    size_t length() const;
    size_t buffer_capacity() const;
    void AppendNewLine();

private:
    std::shared_ptr<TCPConnection> sender_;
    std::shared_ptr<TCPConnection> receiver_;

private:
    size_t buffer_capacity_;
    size_t length_;
    char* data_;
};

#endif //KEY_VALUE_STORE_TCP_MESSAGE_H
