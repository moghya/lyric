//
// Created by Shubham Sawant on 08/07/21.
//

#ifndef KEY_VALUE_STORE_TCP_UTILS_H
#define KEY_VALUE_STORE_TCP_UTILS_H

#include <errno.h>
#include <iostream>
#include <memory>
#include <thread>

#include "../../third_party/spdlog/include/spdlog/spdlog.h"
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
        SPDLOG_DEBUG(fmt::format("Waiting to receive message on socket: {}", socket_fd));
        auto read_length = recv(socket_fd, (void *) message->data(), message->buffer_capacity(), 0 /* flags */);
        if ( read_length < 0 ) {
            SPDLOG_ERROR(fmt::format("recv failed: {}, {} on socket: {}", strerror(errno), errno, socket_fd));
            return nullptr;
        }
        if ( read_length == 0) {
            SPDLOG_ERROR("Connection closed by peer.");
            return nullptr;
        }
        message->set_length(read_length);
        return message;
    }

    static size_t send_stream_message(unsigned int socket_fd,
                                      std::unique_ptr<Message> message) {
        SPDLOG_DEBUG(fmt::format("Sending message: {} to socket: {}", message->data_str(), socket_fd));
        // TODO(moghya) : change this to check for size returned and handle errors set if any.
        auto send_length = send(socket_fd, (void *) message->data(), message->length(), MSG_NOSIGNAL /* flags */);
        if ( send_length <= 0 ) {
            SPDLOG_ERROR(fmt::format("send failed: {}, {} on socket: {}", strerror(errno), errno, socket_fd));
            return 0;
        }
        SPDLOG_INFO(fmt::format("Sent {} bytes", send_length));
        return send_length;
    }

    static void SpawnThread(std::function<void()> cb, bool join_thread = true) {
        auto t = std::make_shared<std::thread>(std::move(cb));
        if (join_thread) {
            t->join();
        } else {
            t->detach();
        }
    }
} // tcp_util
#endif // KEY_VALUE_STORE_TCP_UTILS_H
