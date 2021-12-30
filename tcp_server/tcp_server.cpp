//
// Created by Shubham Sawant on 30/06/21.
//

#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <sys/select.h>
#include <thread>

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

bool TCPServer::BindAndListen() {
  int bind_result = bind(socket_.fd(),  (struct sockaddr*) &address_, sizeof (address_));
  if (bind_result != 0) {
    std::cout << "Could not bind to the port " << port_ << std::endl;
    return false;
  }
  int listen_result = listen(socket_.fd(), backlog_queue_size_);
  if (listen_result != 0) {
    std::cout << "Could not listen on the port " << port_ << std::endl;
    return false;
  }
  std::cout << "Started listening "
            << " on PORT: " << port_
            << std::endl;
  return true;
}


//-----------------------------------------------

void TCPServer::AcceptConnections() {
  fd_set read_fd_set;
  while (is_on_) {
    PopulateReadFdSet(read_fd_set);
    PRINT_THREAD_ID std::cout << "Finding readable socket_fds " << std::endl;
    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
      return;
    }

    // Check if server_socket_fd_ is readable to accept new connection.
    if (FD_ISSET(socket_.fd(), &read_fd_set)) {
      PRINT_THREAD_ID std::cout << "Found readable socket_fd: "
                                << socket_.fd() << std::endl;
      AcceptConnection();
    }

    std::set<std::shared_ptr<TCPConnection>> readable_clients;
    // Check all active sender sockets if message is available
    active_clients_map_lock_.lock();
    for(auto client_entry : active_clients_map_) {
      if (FD_ISSET(client_entry.first, &read_fd_set)) {
        readable_clients.insert(client_entry.second);
      }
    }
    active_clients_map_lock_.unlock();

    for (auto client : readable_clients) {
      PRINT_THREAD_ID std::cout << "Found readable sender socket fd: "
                                << client->socket_fd() << std::endl;
      AcceptMessage(std::move(client));
    }
    readable_clients.clear();
  }
}

void TCPServer::PopulateReadFdSet(fd_set& read_fd_set) {
  FD_ZERO(&read_fd_set);
  // Add server_socket_fd_ to accept new connections.
  FD_SET(socket_.fd(), &read_fd_set);

  // Add all the active sender sockets for getting messages.
  active_clients_map_lock_.lock();
  for(auto client_entry : active_clients_map_) {
    FD_SET(client_entry.first, &read_fd_set);
  }
  active_clients_map_lock_.unlock();
}

void TCPServer::AcceptMessage(std::shared_ptr<TCPConnection> client) {
  std::function<void()> message_handler =
  [this, client = std::move(client)]() {
    HandleMessage(GetMessage(std::move(client)));
  };
  SpawnThread(std::move(message_handler));
}

std::shared_ptr<TCPConnection> TCPServer::AcceptConnection() {
  PRINT_THREAD_ID std::cout << "Accepting new connection" << std::endl;
  auto client = std::make_shared<TCPConnection>();
  // Initialize sender address struct and accept new connection
  unsigned int client_socket_fd = accept(socket_.fd(),
                                         client->address(),
                                         client->address_length_ptr());
  if (client_socket_fd == -1) {
    PRINT_THREAD_ID std::cout << "Invalid sender socket fd" << std::endl;
    return nullptr;
  }
  AddToActiveClient(client_socket_fd, client);
  PRINT_THREAD_ID std::cout << "Connection accepted: "
                            << client->socket_fd() << std::endl;
  return client;
}

void TCPServer::SpawnThread(std::function<void()> cb, bool join_thread) {
  auto t = std::make_shared<std::thread>(std::move(cb));
  if (join_thread) {
    t->join();
  } else {
    t->detach();
  }
}

std::shared_ptr<TCPMessage> TCPServer::GetMessage(
    std::shared_ptr<TCPConnection> client) {
  if (!client) return nullptr;
  auto message = std::move(client->ReceiveMessage(message_buffer_size_));
  message->put_data(message->length(),0);
  if (!message) return nullptr;
  auto tcp_message = std::make_shared<TCPMessage>(client, std::move(message));
  return tcp_message;
}

void TCPServer::HandleMessage(std::shared_ptr<TCPMessage> tcp_message) {
  auto client = tcp_message->sender();
  if (!tcp_message->message()) {
      RemoveFromActiveClients(client);
      PRINT_THREAD_ID std::cout << "Connection lost: " << client->socket_fd() << std::endl;
      return;
  }

  try {
    PRINT_THREAD_ID std::cout << "Client: " << client->socket_fd() << "\t" << "Message: " << tcp_message->message()->data_str() << std::endl;
    auto action_to_take = message_handler_(tcp_message);
    if (action_to_take == tcp_util::ACTION_ON_CONNECTION::CLOSE) {
      PRINT_THREAD_ID std::cout << "Closing client: " << client->socket_fd() << std::endl;
      RemoveFromActiveClients(client);
    }
  } catch(std::exception exp) {
    std::cerr << "failed to execute [OnMessage] function";
  }
}
