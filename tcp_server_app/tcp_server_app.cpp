//
// Created by Shubham Sawant on 08/07/21.
//

#include "tcp_server_app.h"

TCPServerApp::TCPServerApp(std::string name, unsigned int port):
      name_(name) {
      TCPMessage::Handler app_message_handler =
        [this](std::shared_ptr<TCPMessage> message) -> tcp_util::ACTION_ON_CONNECTION {
        return this->HandleMessage(message);
      };
      server_ = std::make_shared<TCPServer>(
                    port, app_message_handler,
  GetMessageBufferCapacity());
}

TCPServerApp::~TCPServerApp() {} // Keep compiler happy :)

tcp_util::ACTION_ON_CONNECTION
TCPServerApp::HandleMessage(std::shared_ptr<TCPMessage> tcp_message) {
    PRINT_THREAD_ID
    std::cout << name_ << ":: Received message: " << tcp_message->message()->data()
              << " from: " << tcp_message->sender()->socket_fd() << "\n";
    tcp_message->sender()->SendMessage("Thanks for message, closing your connection.");
    return tcp_util::ACTION_ON_CONNECTION::CLOSE;
}

void TCPServerApp::Start() {
    server_->StartListening();
}