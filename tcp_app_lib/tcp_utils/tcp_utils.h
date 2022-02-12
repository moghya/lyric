//
// Created by Shubham Sawant on 08/07/21.
//

#ifndef KEY_VALUE_STORE_TCP_UTILS_H
#define KEY_VALUE_STORE_TCP_UTILS_H

#include <errno.h>
#include <iostream>
#include <memory>
#include <set>
#include <thread>
#include <unordered_set>

#include "../../third_party/spdlog/include/spdlog/spdlog.h"
#include "message.h"


#define PRINT_THREAD_ID do { \
   std::cout << "Thread::" \
             << std::this_thread::get_id() << "::" << __FUNCTION__ \
             << "\t"; \
  } while(false); \

#define GET_SOCKET_ERROR Error(errno, strerror(errno));

namespace tcp_util {
    enum ACTION_ON_CONNECTION {
        KEEP_OPEN,
        CLOSE,
    };

    enum ErrorType {
        kNoError = 0,
        kUnknown = 1,
        kBrokenPipe = 2,
        kBadFileDescriptor = 3,
        kDisconnectedConnection = 4,
        kTimeout = 5,
    };

    static ErrorType GetErrorTypeFromErrorNo(int error_no) {
        switch(error_no) {
            case ErrorType::kNoError:     return ErrorType::kNoError;
            case EPIPE: return ErrorType::kBrokenPipe;
            case EBADF: return ErrorType::kBadFileDescriptor;
        }
        return ErrorType::kUnknown;
    }

    class Error {
        public:
            Error(int err_no = ErrorType::kNoError, std::string err_msg = "") :
                type_(tcp_util::GetErrorTypeFromErrorNo(err_no)),
                msg_(err_msg) {
            }
            ~Error() {}
             std::string to_string() const {
                return "<<Err::"+std::to_string(type_)+"::"+msg_+">>";
             }
             const tcp_util::ErrorType type() const {
                 return type_;
             }

        private:
            tcp_util::ErrorType type_;
            std::string msg_;
    };

    template<typename T>
    struct OperationResult {
        bool success_;
        Error error_;
        T result_;
    };

    static OperationResult<std::unique_ptr<Message>>
    receive_stream_message(unsigned int socket_fd, unsigned int buffer_size) {
        auto message = std::make_unique<Message>(buffer_size);
        SPDLOG_DEBUG(fmt::format("Waiting to receive message on socket: {}", socket_fd));
        auto read_length = recv(socket_fd, (void *) message->data(), message->buffer_capacity(), 0 /* flags */);

        if (read_length < 0) {
            auto err = GET_SOCKET_ERROR
            SPDLOG_ERROR(fmt::format("recv failed: {} on socket: {}",  err.to_string(), socket_fd));
            return {false, err, nullptr};
        }else if (read_length == 0) {
            auto err = Error(ErrorType::kDisconnectedConnection);
            SPDLOG_INFO(fmt::format("Connection closed by peer on socket:{}",socket_fd));
            return {false, err, nullptr};
        } else {
            SPDLOG_DEBUG("Read {} bytes.", read_length);
            message->set_length(read_length);
            message->put_data(read_length,0);
            return {true, Error(), std::move(message)};
        }
    }

    static OperationResult<int>
    send_stream_message(unsigned int socket_fd, std::unique_ptr<Message> message) {
        SPDLOG_DEBUG(fmt::format("Sending message: {} to socket: {}", message->data_str(), socket_fd));
        // TODO(moghya) : change this to check for size returned and handle errors set if any.
        int send_length = send(socket_fd, (void *) message->data(), message->length(), MSG_NOSIGNAL /* flags */);
        Error err(0);
        bool success = true;
        if (send_length <= 0) {
            success = false;
            err = GET_SOCKET_ERROR
            SPDLOG_ERROR(fmt::format("send failed: {} on socket: {} for message: {}",
                                     err.to_string(), socket_fd, message->data_str()));
        } else {
            SPDLOG_DEBUG(fmt::format("Sent {} bytes", send_length));
        }
        return {success, err, send_length};
    }

    static std::shared_ptr<const std::thread> SpawnThread(std::function<void()> cb, bool join_thread = true) {
        auto t = std::make_shared<std::thread>(std::move(cb));
        if (join_thread) {
            t->join();
        } else {
            t->detach();
        }
        return t;
    }

    static OperationResult<std::set<unsigned int>>
    get_readable_fds_using_select(std::unordered_set<unsigned int>& read_fds,
                                  struct timeval* timeout) {
        std::set<unsigned int> readable_fds;

        fd_set read_fd_set;
        FD_ZERO(&read_fd_set);
        for(auto fd:read_fds) {
            FD_SET(fd, &read_fd_set);
        }

        int select_ret = select(FD_SETSIZE, &read_fd_set, NULL, NULL, timeout);
        if (select_ret < 0) {
            auto err = GET_SOCKET_ERROR
            SPDLOG_ERROR(fmt::format("select failed: {}. Shutting down.",  err.to_string()));
            return {false, err, readable_fds};
        } else if (select_ret == 0) {
            SPDLOG_DEBUG("Hit select timeout.");
            return {false, Error(ErrorType::kTimeout), readable_fds};
        }

        for(auto fd : read_fds) {
            if (FD_ISSET(fd, &read_fd_set)) {
                SPDLOG_INFO(fmt::format("Found readable socket_fd: {}", fd));
                readable_fds.insert(fd);
            }
        }
        return {true, Error(), readable_fds};
    }
} // tcp_util
#endif // KEY_VALUE_STORE_TCP_UTILS_H
