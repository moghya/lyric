//
// Created by Shubham Sawant on 08/07/21.
//

#ifndef KEY_VALUE_STORE_TCP_UTILS_H
#define KEY_VALUE_STORE_TCP_UTILS_H

#include <cstring>      
#include <iostream>
#include <memory>
#include <netdb.h>      
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <thread>
#include <unistd.h>

#include "message.h"
#include "tcp_connection.h"


#define PRINT_THREAD_ID do { \
                            std::cout << "Thread::" \
                                      << std::this_thread::get_id() \
                                      << "::" << __FUNCTION__ << \
        "\t"; \
  } while(false); \

enum ACTION_ON_CONNECTION {
    KEEP_OPEN,
    CLOSE,
};


class TCPMessage {
public:
    TCPMessage(std::shared_ptr<TCPConnection> sender,
               std::shared_ptr<Message> message) :
    sender_(sender),
    message_(message) {}
    const std::shared_ptr<TCPConnection>& sender() const {
        return sender_;
    }
    const std::shared_ptr<Message>& message() const {
        return message_;
    }
private:
    std::shared_ptr<TCPConnection> sender_;
    std::shared_ptr<Message> message_;
};
#endif // KEY_VALUE_STORE_TCP_UTILS_H
