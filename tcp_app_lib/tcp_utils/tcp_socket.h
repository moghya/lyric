//
// Created by Shubham Sawant on 20/12/21.
//

#ifndef KEY_VALUE_STORE_TCP_SOCKET_H
#define KEY_VALUE_STORE_TCP_SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>

class TCPSocket {
public:
    TCPSocket(sa_family_t family);
    ~TCPSocket();
    const unsigned int fd() const {
        return fd_;
    }
private:
    int fd_ = -1;
};


#endif//KEY_VALUE_STORE_TCP_SOCKET_H
