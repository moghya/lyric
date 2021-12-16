//
// Created by Shubham Sawant on 08/07/21.
//


#include "tcp_utils.h"


size_t ReceiveMessage(std::shared_ptr<TCPMessage> message) {
  auto sender = message->sender();
  auto message_length = recvfrom(sender->socket_fd(),
                                 (void*) message->data(),
                                 message->buffer_capacity(),
                                 0,
                                 sender->address(),
                                 sender->address_length_ptr());
  message->set_length(message_length);
  message->put_data(message_length, 0);
  return message_length;
}

size_t SendMessage(std::shared_ptr<TCPMessage> message) {
  auto receiver = message->receiver();
  return sendto(receiver->socket_fd(),
                (void*) message->data(),
                message->length(),
                0,
                receiver->address(),
                receiver->address_length());
}