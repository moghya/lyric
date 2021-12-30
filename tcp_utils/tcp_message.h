//
// Created by Shubham Sawant on 16/12/21.
//

#ifndef KEY_VALUE_STORE_TCP_MESSAGE_H
#define KEY_VALUE_STORE_TCP_MESSAGE_H

#include "message.h"
#include "tcp_connection.h"
#include "tcp_utils.h"

class TCPMessage {
public:
    TCPMessage(std::shared_ptr<TCPConnection> sender,
               std::unique_ptr<Message> message);
    const std::shared_ptr<TCPConnection>& sender() const {
        return sender_;
    }
    const Message* message()  {
        return message_.get();
    }
    ~TCPMessage();
    typedef std::function<tcp_util::ACTION_ON_CONNECTION(std::shared_ptr<TCPMessage>)> Handler;
private:
    std::shared_ptr<TCPConnection> sender_;
    std::unique_ptr<Message> message_;
};

#endif//KEY_VALUE_STORE_TCP_MESSAGE_H
