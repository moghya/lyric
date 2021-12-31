//
// Created by Shubham Sawant on 30/12/21.
//

#ifndef KEY_VALUE_STORE_TCPSERVERAPPCLIENT_H
#define KEY_VALUE_STORE_TCPSERVERAPPCLIENT_H


#include <string>
#include "../tcp_client/tcp_client.h"

class TCPServerAppClient {
public:
    TCPServerAppClient(std::string ip_address,
                       unsigned int port,
                       std::string name);
    ~TCPServerAppClient();

    const std::shared_ptr<const TCPClient> tcp_client() const {
        return tcp_client_;
    }

protected:
    std::shared_ptr<TCPClient> tcp_client_;
};


#endif//KEY_VALUE_STORE_TCPSERVERAPPCLIENT_H
