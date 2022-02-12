//
// Created by Shubham Sawant on 30/06/21.
//

#include <errno.h>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <sys/select.h>
#include <thread>
#include <unordered_set>

#include "../../third_party/spdlog/include/spdlog/spdlog.h"

#include "tcp_server.h"


TCPServer::TCPServer(unsigned int port,
                     TCPMessage::Handler message_handler,
                     unsigned int message_buffer_size,
                     unsigned int backlog_queue_size) :
    port_(port),
    message_handler_(message_handler),
    message_buffer_size_(message_buffer_size),
    backlog_queue_size_(backlog_queue_size),
    socket_(AF_INET) {
    bzero(&address_, sizeof (address_));
    address_.sin_family      = AF_INET;
    address_.sin_addr.s_addr = htonl(INADDR_ANY);
    address_.sin_port        = htons(port_);
}

TCPServer::~TCPServer() {
    SPDLOG_INFO("Destroying server");
}

void TCPServer::StartListening() {
  is_on_ = true;
  if (!BindAndListen()) {
    return;
  }
  tcp_util::SpawnThread([this](){AcceptMessages();}, false);
  AcceptConnections();
}

void TCPServer::StopListening() {
    is_on_ = false;
}

bool TCPServer::BindAndListen() {
  int bind_result = bind(socket_.fd(),  (struct sockaddr*) &address_, sizeof (address_));
  if (bind_result != 0) {
      SPDLOG_INFO(fmt::format("Could not bind to the port: {}", port_));
    return false;
  }
  int listen_result = listen(socket_.fd(), backlog_queue_size_);
  if (listen_result != 0) {
    SPDLOG_INFO(fmt::format("Could not listen on the port: {}", port_));
    return false;
  }
  SPDLOG_INFO(fmt::format("Started listening on port: {}", port_));
  return true;
}


//-----------------------------------------------

void TCPServer::AcceptConnections() {
  struct timeval select_wait_time{5,0};
  while (is_on_) {
      std::unordered_set<unsigned int> read_fds{socket_.fd()};
      auto select_res = tcp_util::get_readable_fds_using_select(read_fds, &select_wait_time);
      if (!select_res.success_) {
          //TODO(moghya): Handle each error type separately.
          SPDLOG_ERROR(fmt::format("Select readable fds failed due to error: {}", select_res.error_.to_string()));
          continue;
      }
      AcceptConnection();
  }
  SPDLOG_INFO("Server is not on, shutting down...");
}

void TCPServer::PopulateSetWithConnectedClients(std::unordered_set<unsigned int>& fd_set) {
  // Add all the active sender sockets for getting messages.
  active_clients_map_lock_.lock();
  for(auto client_entry : active_clients_map_) {
    fd_set.insert(client_entry.first);
  }
  active_clients_map_lock_.unlock();
}

void TCPServer::AcceptMessages() {
    std::unordered_set<unsigned int> read_fd_set;
    std::unordered_set<std::shared_ptr<TCPConnection>> readable_clients;
    struct timeval select_wait_time{5,0};
    while (is_on_) {
        PopulateSetWithConnectedClients(read_fd_set);
        auto select_res = tcp_util::get_readable_fds_using_select(read_fd_set, &select_wait_time);
        if (!select_res.success_) {
            //TODO(moghya): Handle each error type separately.
            SPDLOG_ERROR(fmt::format("Select readable fds failed due to error: {}", select_res.error_.to_string()));
            continue;
        }

        // extract as a method
        active_clients_map_lock_.lock();
        for(auto fd : select_res.result_) {
            auto it = active_clients_map_.find(fd);
            if (it!=active_clients_map_.end()) {
                SPDLOG_INFO(fmt::format("Found readable socket_fd: {}", it->first));
                readable_clients.insert(it->second);
            }
        }
        active_clients_map_lock_.unlock();

        for(auto client : readable_clients) {
            RemoveFromActiveClients(client);
            AddToHandlingClients(client);
            AcceptMessage(client);
        }

        readable_clients.clear();
        read_fd_set.clear();
    }
    SPDLOG_INFO("Server is not on, shutting down...");
}

void TCPServer::AcceptMessage(std::shared_ptr<TCPConnection> client, bool spawn_thread) {
  auto client_socket_fd = client->socket_fd();
  SPDLOG_INFO(fmt::format("Accepting message from sender socket_fd: {}", client_socket_fd));
  auto message_handler = [this, client = std::move(client)]() {
    HandleMessage(GetMessage(std::move(client)));
  };
  if (!spawn_thread) {
      message_handler();
  } else {
     auto t = tcp_util::SpawnThread(std::move(message_handler), false);
  }
}

std::shared_ptr<TCPConnection> TCPServer::AcceptConnection() {
  SPDLOG_INFO("Accepting new connection");
  auto client = std::make_shared<TCPConnection>();
  // Initialize sender address struct and accept new connection
  int client_socket_fd = accept(socket_.fd(),
                                client->address(),
                                client->address_length_ptr());
  if (client_socket_fd < 0) {
      SPDLOG_ERROR(fmt::format("accept failed: {}", tcp_util::Error(errno).to_string()));
      return nullptr;
  }
  client->set_socket_fd(client_socket_fd);
  AddToActiveClients(client);
  SPDLOG_INFO(fmt::format("Connection accepted on socket_fd: {}", client->socket_fd()));
  return client;
}

std::shared_ptr<TCPMessage> TCPServer::GetMessage(
    std::shared_ptr<TCPConnection> client) {
  if (!client) return nullptr;
  auto res = client->ReceiveMessage(message_buffer_size_);
  if(res.result_) res.result_->put_data(res.result_->length(), 0);
  auto tcp_message = std::make_shared<TCPMessage>(client, std::move(res.result_));
  return tcp_message;
}

void TCPServer::HandleMessage(std::shared_ptr<TCPMessage> tcp_message) {
  auto client = tcp_message->sender();
  if (!tcp_message->message()) {
      RemoveFromHandlingClients(client);
      SPDLOG_INFO(fmt::format("Connection removed from socket_fd: {}", client->socket_fd()));
      return;
  }

  try {
    SPDLOG_INFO(fmt::format("Client socket_fd: {} Message: {}",
                             client->socket_fd(),
                             tcp_message->message()->data_str()));
    auto action_to_take = message_handler_(tcp_message);
    RemoveFromHandlingClients(client);
    if (action_to_take == tcp_util::ACTION_ON_CONNECTION::CLOSE) {
      SPDLOG_INFO(fmt::format("Closing client socket_fd: {}", client->socket_fd()));
      return;
    }
    AddToActiveClients(client);
  } catch(std::exception exp) {
      SPDLOG_ERROR("failed to execute [OnMessage] function: ", exp.what());
  }
}
