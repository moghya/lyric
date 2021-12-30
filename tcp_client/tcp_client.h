//
// Created by Shubham Sawant on 19/12/21.
//

#ifndef KEY_VALUE_STORE_TCP_CLIENT_H
#define KEY_VALUE_STORE_TCP_CLIENT_H

#include <vector>
#include <unordered_map>

#include "../tcp_utils/tcp_connection.h"
#include "../tcp_utils/tcp_message.h"


class TCPClient {
public:
    TCPClient();
    ~TCPClient();
    std::shared_ptr<const TCPConnection> AddConnection(std::string ip_address,
                                                       unsigned int port,
                                                       std::string name);
    std::shared_ptr<const TCPConnection> GetConnection(unsigned int index) const;
    std::shared_ptr<const TCPConnection> GetConnectionByName(const std::string& name) const;

private:
    std::vector<std::shared_ptr<TCPConnection>> connections_;
    std::unordered_map<std::string, int> connections_map_;
};


#endif//KEY_VALUE_STORE_TCP_CLIENT_H
