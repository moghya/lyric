//
// Created by Shubham Sawant on 08/07/21.
//

#ifndef KEY_VALUE_STORE_TCP_SERVER_H
#define KEY_VALUE_STORE_TCP_SERVER_H

#include <cstring>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <unordered_map>

#include "../tcp_utils/tcp_connection.h"
#include "../tcp_utils/tcp_message.h"
#include "../tcp_utils/tcp_socket.h"
#include "../tcp_utils/tcp_utils.h"

/*
 * This class implements methods to listen, accept connections.
 * It also reads messages and gives them to app_ of type TCPServerApp for
 * handling.
 * Uses select to monitor fds to accept connection or message.
 * New thread is spawned for processing a new message.
 * */
class TCPServer {
public:
  TCPServer(unsigned int port,
            TCPMessage::Handler message_handler,
            unsigned int message_buffer_size = 512,
            unsigned int backlog_queue_size = 1000);
  ~TCPServer();
  void StartListening();
protected:
    bool BindAndListen();
    void AcceptConnections();
    std::shared_ptr<TCPConnection> AcceptConnection();
    void PopulateFdSetWithConnectedClients(fd_set& fd_set);
    void AcceptMessage(std::shared_ptr<TCPConnection> client);
    std::shared_ptr<TCPMessage> GetMessage(std::shared_ptr<TCPConnection> client);
    void HandleMessage(std::shared_ptr<TCPMessage> message);
    void SpawnThread(std::function<void()> cb, bool join_thread = true);

private:
  void AddToActiveClient(unsigned int client_socket_fd,
                         std::shared_ptr<TCPConnection> client) {
    client->set_socket_fd(client_socket_fd);
    active_clients_map_.insert({client_socket_fd, client});
  }

  void RemoveFromActiveClients(std::shared_ptr<TCPConnection> client) {
    close(client->socket_fd());
    active_clients_map_lock_.lock();
    active_clients_map_.erase(client->socket_fd());
    active_clients_map_lock_.unlock();
  }

private:
  unsigned int port_;
  TCPMessage::Handler message_handler_;
  unsigned int message_buffer_size_;
  unsigned int backlog_queue_size_;
  TCPSocket socket_;
  struct sockaddr_in address_;

  // Client socket_fd to TCPConnection map.
  // TODO(sawant) : Prune this to avoid memory leak due to abruptly closed
  //  connections by client.
  std::unordered_map<unsigned int, std::shared_ptr<TCPConnection>>
      active_clients_map_;
  std::mutex active_clients_map_lock_;

  fd_set active_clients_fd_set_;
  bool is_on_ = false;
};


#endif //KEY_VALUE_STORE_TCP_SERVER_H
