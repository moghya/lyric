//
// Created by Shubham Sawant on 08/07/21.
//


#include "tcp_utils.h"


size_t ReceiveMessage(std::shared_ptr<TCPMessage> message) {
  auto message_length = recvfrom(message->sender()->socket_fd(),
                                 (void*) message->data(),
                                 message->buffer_capacity(),
                                 0,
                                 message->sender()->address(),
                                 message->sender()->address_length_ptr());
  message->set_length(message_length);
  message->put_data(message_length, 0);
  return message_length;
}

size_t SendMessage(std::shared_ptr<TCPMessage> message, bool append_new_line) {
  if (append_new_line) {
    message->put_data(message->length(),'\n');
    message->set_length(message->length()+1);
  }
  return sendto(message->sender()->socket_fd(),
                (void*) message->data(),
                message->length(),
                0,
                message->sender()->address(),
                message->sender()->address_length());
}