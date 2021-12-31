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
    TCPClient(std::string dest_ip_address,
              unsigned int dest_port,
              std::string name);
    ~TCPClient();

    const std::string name() const {
        return name_;
    }

    bool SendMessage(std::string message) const {
        return connection_->SendMessage(message);
    }

    std::unique_ptr<Message> ReceiveMessage(size_t buffer_capacity) const {
        return std::move(connection_->ReceiveMessage(buffer_capacity));
    }

private:
    std::string dest_ip_address_;
    unsigned int dest_port_;
    std::string name_;
    std::shared_ptr<TCPConnection> connection_;
};


#endif//KEY_VALUE_STORE_TCP_CLIENT_H
