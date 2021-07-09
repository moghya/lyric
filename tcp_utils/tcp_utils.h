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


#define PRINT_THREAD_ID do { \
                            std::cout << "Thread::" \
                                      << std::this_thread::get_id() \
                                      << "::" << __FUNCTION__ << \
        "\t"; \
  } while(false); \

/*
 * This class is a wrapper around client socket fd and socket address.
 * */
class ClientConnection {
public:
  ClientConnection();
  ~ClientConnection();
  struct sockaddr* address() const;
  socklen_t* address_length_ptr() const;
  size_t address_length() const;
  void set_socket_fd(unsigned int sokect_fd);
  unsigned int socket_fd() const;

private:
  struct sockaddr* address_;
  socklen_t* address_length_;
  unsigned int socket_fd_;
};

enum ACTION_ON_CONNECTION {
    KEEP_OPEN,
    CLOSE,
};

/*
 * This class is a wrapper around message buffer filled using the data received
 * from client.
 * */
class Message {
public:
  Message(std::shared_ptr<ClientConnection> client,
          size_t buffer_capacity);
  ~Message();
  std::shared_ptr<ClientConnection> client() const;
  char* data() const;
  void put_data(unsigned int index, char c);
  void set_data(std::string data);
  const std::string data_str() const;
  void set_length(size_t length);
  size_t length() const;
  size_t buffer_capacity() const;

private:
  std::shared_ptr<ClientConnection> client_;
  size_t buffer_capacity_;
  size_t length_;
  char* data_;
};

/*
 * Following methods are wrapper around recvfrom and sendto methods.
 * */
size_t ReceiveMessage(std::shared_ptr<Message> message);
size_t SendMessage(std::shared_ptr<Message> message,
                   bool append_new_line = true);

#endif // KEY_VALUE_STORE_TCP_UTILS_H
