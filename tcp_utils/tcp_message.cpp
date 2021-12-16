//
// Created by Shubham Sawant on 16/12/21.
//

#include <memory>
#include "tcp_message.h"

TCPMessage::TCPMessage(std::shared_ptr<TCPConnection> sender,
                       std::unique_ptr<Message> message) :
           sender_(sender),
           message_(std::move(message)) {}

TCPMessage::~TCPMessage() {
}