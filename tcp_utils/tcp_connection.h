//
// Created by Shubham Sawant on 16/12/21.
//

#ifndef KEY_VALUE_STORE_TCP_CONNECTION_H
#define KEY_VALUE_STORE_TCP_CONNECTION_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

/*
 * This class is a wrapper around tcp socket fd and socket address.
 * */
class TCPConnection {
public:
    TCPConnection();
    ~TCPConnection();
    struct sockaddr* address() const;
    socklen_t* address_length_ptr() const;
    size_t address_length() const;
    void set_socket_fd(unsigned int sokect_fd);
    unsigned int socket_fd() const;
private:
    struct sockaddr* address_;
    socklen_t* address_length_;
    unsigned int socket_fd_;
};


#endif //KEY_VALUE_STORE_TCP_CONNECTION_H
