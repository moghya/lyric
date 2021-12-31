//
// Created by Shubham Sawant on 08/07/21.
//

#ifndef KEY_VALUE_STORE_TCP_UTILS_H
#define KEY_VALUE_STORE_TCP_UTILS_H

#include <iostream>
#include <memory>
#include <thread>

#include "message.h"

#define PRINT_THREAD_ID do { \
   std::cout << "Thread::" \
             << std::this_thread::get_id() << "::" << __FUNCTION__ \
             << "\t"; \
  } while(false); \

namespace tcp_util {
    enum ACTION_ON_CONNECTION {
        KEEP_OPEN,
        CLOSE,
    };

    static std::unique_ptr<Message> receive_stream_message(unsigned int socket_fd,
                                                           unsigned int buffer_size)
    {
        auto message = std::make_unique<Message>(buffer_size);
        if (recv(socket_fd, (void *) message->data(), message->buffer_capacity(), 0 /* flags */) <=0 ) {
            return nullptr;
        }
        return message;
    }

    static size_t send_stream_message(unsigned int socket_fd,
                                      std::unique_ptr<Message> message) {
        // TODO(moghya) : change this to check for size returned and handle errors set if any.
        return send(socket_fd, (void *) message->data(), message->length(), MSG_NOSIGNAL /* flags */);
    }
} // tcp_util
#endif // KEY_VALUE_STORE_TCP_UTILS_H
