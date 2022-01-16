//
// Created by Shubham Sawant on 30/06/21.
//

#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <sys/select.h>
#include <thread>

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
}

void TCPServer::StartListening() {
  is_on_ = true;
  if (!BindAndListen()) {
    return;
  }
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
  fd_set read_fd_set;
  fd_set error_fd_set;
  struct timeval select_wait_time{5,0};
  while (is_on_) {
    PopulateFdSetWithConnectedClients(read_fd_set);
    PopulateFdSetWithConnectedClients(error_fd_set);

    // Add server's socket_fd to read_fd_set for accepting new connections.
    FD_SET(socket_.fd(), &read_fd_set);

    SPDLOG_INFO("Finding readable socket_fds...");
    int select_ret = select(FD_SETSIZE, &read_fd_set, NULL,
                            &error_fd_set, &select_wait_time);
    if (select_ret < 0) {
      SPDLOG_ERROR("Error in select. Shutting down...");
      return;
    }

    if (select_ret == 0) {
        SPDLOG_INFO("Hit select timeout, continuing...");
        continue;
    }

    // Check if server_socket_fd_ is readable to accept new connection.
    if (FD_ISSET(socket_.fd(), &read_fd_set)) {
      SPDLOG_INFO("New connection available!");
      AcceptConnection();
    }

    std::set<std::shared_ptr<TCPConnection>> readable_clients;
    // Check all active sender sockets if message is available
    active_clients_map_lock_.lock();

    // Remove all clients with error
    for(auto client_entry : active_clients_map_) {
      if (FD_ISSET(client_entry.first, &error_fd_set)) {
        SPDLOG_ERROR(fmt::format("Found error socket_fd: {}", socket_.fd()));
        RemoveFromActiveClients(client_entry.second);
      }
    }
    for(auto client_entry : active_clients_map_) {
      if (FD_ISSET(client_entry.first, &read_fd_set)) {
        SPDLOG_INFO(fmt::format("Found readable socket_fd: {}", client_entry.first));
        readable_clients.insert(client_entry.second);
      }
    }
    active_clients_map_lock_.unlock();
    for(auto client : readable_clients) {
        this->AcceptMessage(client);
    }
    readable_clients.clear();
  }
  SPDLOG_INFO("Server is not on, shutting down...");
}

void TCPServer::PopulateFdSetWithConnectedClients(fd_set& fd_set) {
  FD_ZERO(&fd_set);
  // Add all the active sender sockets for getting messages.
  active_clients_map_lock_.lock();
  for(auto client_entry : active_clients_map_) {
    FD_SET(client_entry.first, &fd_set);
  }
  active_clients_map_lock_.unlock();
}

void TCPServer::AcceptMessage(std::shared_ptr<TCPConnection> client, bool spawn_thread) {
  SPDLOG_INFO(fmt::format("Accepting message from sender socket_fd: {}", client->socket_fd()));
  auto message_handler = [this, client = std::move(client)]() {
    HandleMessage(GetMessage(std::move(client)));
  };
  if (!spawn_thread) {
      message_handler();
  } else {
      tcp_util::SpawnThread(std::move(message_handler), false);
  }
}

std::shared_ptr<TCPConnection> TCPServer::AcceptConnection() {
  SPDLOG_INFO("Accepting new connection");
  auto client = std::make_shared<TCPConnection>();
  // Initialize sender address struct and accept new connection
  unsigned int client_socket_fd = accept(socket_.fd(),
                                         client->address(),
                                         client->address_length_ptr());
  if (client_socket_fd == -1) {
    SPDLOG_ERROR("Invalid sender socket fd");
    return nullptr;
  }
  AddToActiveClient(client_socket_fd, client);
  SPDLOG_INFO(fmt::format("Connection accepted on socket_fd: {}", client->socket_fd()));
  return client;
}

std::shared_ptr<TCPMessage> TCPServer::GetMessage(
    std::shared_ptr<TCPConnection> client) {
  if (!client) return nullptr;
  auto message = std::move(client->ReceiveMessage(message_buffer_size_));
  if (message) message->put_data(message->length(),0);
  auto tcp_message = std::make_shared<TCPMessage>(client, std::move(message));
  return tcp_message;
}

void TCPServer::HandleMessage(std::shared_ptr<TCPMessage> tcp_message) {
  auto client = tcp_message->sender();
  if (!tcp_message->message()) {
      RemoveFromActiveClients(client);
      SPDLOG_ERROR(fmt::format("Connection lost from socket_fd: {}", client->socket_fd()));
      return;
  }

  try {
    SPDLOG_INFO(fmt::format("Client socket_fd: {} Message: {}",
                             client->socket_fd(),
                             tcp_message->message()->data_str()));
    auto action_to_take = message_handler_(tcp_message);
    if (action_to_take == tcp_util::ACTION_ON_CONNECTION::CLOSE) {
      SPDLOG_INFO(fmt::format("Closing client socket_fd: {}", client->socket_fd()));
      RemoveFromActiveClients(client);
    }
  } catch(std::exception exp) {
    std::cerr << "failed to execute [OnMessage] function";
  }
}
