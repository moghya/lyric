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

#include "tcp_connection.h"
#include "tcp_message.h"



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

/*
 * Following methods are wrapper around recvfrom and sendto methods.
 * */
size_t ReceiveMessage(std::shared_ptr<TCPMessage> message);
size_t SendMessage(std::shared_ptr<TCPMessage> message,
                   bool append_new_line = true);

#endif // KEY_VALUE_STORE_TCP_UTILS_H
