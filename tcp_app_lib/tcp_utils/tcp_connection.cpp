//
// Created by Shubham Sawant on 16/12/21.
//


#include <memory>
#include <string>

#include "tcp_connection.h"
#include "tcp_utils.h"



TCPConnection::TCPConnection() {
    address_ = (struct sockaddr*) malloc(sizeof(struct sockaddr_in));
    address_length_ = (socklen_t*) malloc(sizeof(socklen_t));
}


TCPConnection::TCPConnection(std::string dest_ip_address,
                             unsigned int dest_port) :
    peer_ip_address_(dest_ip_address),
    peer_port_(dest_port){
    ConnectToPeer(peer_ip_address_, peer_port_);
}

bool TCPConnection::ConnectToPeer(std::string dest_ip_address,
                                  unsigned int dest_port) {
    // TODO(moghya): move this to tcp_utils
    struct addrinfo hints;
    struct addrinfo *result;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int rc = getaddrinfo(dest_ip_address.c_str(), std::to_string(dest_port).c_str(), &hints, &result);
    if (rc != 0) {
        SPDLOG_ERROR(fmt::format("getaddrinfo failed: {}",  tcp_util::Error(errno).to_string()));
    }
    struct addrinfo* rp;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socket_fd_ = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_fd_ < 0) {
            SPDLOG_ERROR(fmt::format("socket failed: {}, Continuing...",  tcp_util::Error(errno).to_string()));
            continue;
        }
        if (connect(socket_fd_, rp->ai_addr, rp->ai_addrlen) < 0) {
            SPDLOG_ERROR(fmt::format("connect failed: {}",  tcp_util::Error(errno).to_string()));
            close(socket_fd_);
            continue;
        } else {
            address_ = (struct sockaddr*) malloc(sizeof(struct sockaddr_in));
            memset(address_, 0, sizeof(struct sockaddr));
            *address_ = *(rp->ai_addr);
            address_length_ = (socklen_t*) malloc(sizeof(socklen_t));
            *address_length_ = sizeof (address_);
            initiator_ = true;
            SPDLOG_INFO("connect succeeded");
            break;
        }
    }
    freeaddrinfo(result);
    if (address_ == nullptr) {
        SPDLOG_ERROR("Could not create connection.");
        return false;
    }
    return true;
}

TCPConnection::~TCPConnection() {
    free(address_);
    free(address_length_);
    if (socket_fd_ >= 0) close(socket_fd_);
}

struct sockaddr* TCPConnection::address() const {
    return address_;
}

socklen_t* TCPConnection::address_length_ptr() const {
    return address_length_;
}

size_t TCPConnection::address_length() const {
    return sizeof(address_);
}

void TCPConnection::set_socket_fd(int socket_fd) {
    socket_fd_ = socket_fd;
}

int TCPConnection::socket_fd() const {
    return socket_fd_;
}

tcp_util::OperationResult<std::unique_ptr<Message>>
TCPConnection::ReceiveMessage(size_t buffer_capacity) const {
    auto res = tcp_util::receive_stream_message(
            this->socket_fd(), buffer_capacity);
//    if (res.success_) {
//        res.result_->put_data(res.result_->length(), 0);
//    }
    return res;
}

tcp_util::OperationResult<int>
TCPConnection::SendMessage(std::string message_data) const {
    auto message = std::make_unique<Message>(message_data);
    auto res  = tcp_util::send_stream_message(
            this->socket_fd(), std::move(message));
    return res;
}

bool TCPConnection::initiator() const {
    return initiator_;
}