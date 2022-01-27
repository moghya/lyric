//
// Created by Shubham Sawant on 16/12/21.
//

#ifndef KEY_VALUE_STORE_TCP_CONNECTION_H
#define KEY_VALUE_STORE_TCP_CONNECTION_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "message.h"

enum TCPConnectionState {
    CREATED,
    IDLE_OPEN, // on connection
    IDLE_CLOSED, // on connection closed
    WAITING_FOR_MESSAGE, // waiting for message
    SENDING_MESSAGE, // sending message
};

/*
 * This class is a wrapper around tcp socket fd and socket address.
 * */
class TCPConnection {
public:
    TCPConnection();
    TCPConnection(struct sockaddr* address, unsigned int socket_fd);
    ~TCPConnection();
    struct sockaddr* address() const;
    socklen_t* address_length_ptr() const;
    size_t address_length() const;
    void set_socket_fd(unsigned int sokect_fd);
    unsigned int socket_fd() const;
    /*
     * Following methods are wrapper around recv and send methods.
     * */
    std::unique_ptr<Message> ReceiveMessage(size_t buffer_capacity);
    bool SendMessage(std::string message_data) const;
private:
    struct sockaddr* address_;
    socklen_t* address_length_;
    int socket_fd_ = -1;
};


#endif //KEY_VALUE_STORE_TCP_CONNECTION_H
