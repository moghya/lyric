//
// Created by Shubham Sawant on 16/12/21.
//

#ifndef KEY_VALUE_STORE_TCP_CONNECTION_H
#define KEY_VALUE_STORE_TCP_CONNECTION_H

#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

#include "message.h"
#include "tcp_utils.h"

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
    TCPConnection(std::string peer_ip_address,
                  unsigned int peer_port);
    ~TCPConnection();
    struct sockaddr* address() const;
    socklen_t* address_length_ptr() const;
    size_t address_length() const;
    void set_socket_fd(int socket_fd);
    int socket_fd() const;
    bool initiator() const;
    /*
     * Following methods are wrapper around recv and send methods.
     * */
    tcp_util::OperationResult<std::unique_ptr<Message>>
    ReceiveMessage(size_t buffer_capacity) const;

    tcp_util::OperationResult<int>
    SendMessage(std::string message_data) const;

private:
    bool ConnectToPeer(std::string ip_address, unsigned int port);

private:
    struct sockaddr* address_ = nullptr;
    socklen_t* address_length_ = nullptr;
    int socket_fd_ = -1;
    bool initiator_ = false;

    std::string peer_ip_address_;
    unsigned int peer_port_;
};


#endif //KEY_VALUE_STORE_TCP_CONNECTION_H
