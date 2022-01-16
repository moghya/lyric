//
// Created by Shubham Sawant on 08/07/21.
//

#include "../../third_party/spdlog/include/spdlog/spdlog.h"

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
   SPDLOG_INFO(fmt::format("App: {} - Received message: {} from: {}",
                            name_,
                            tcp_message->message()->data(),
                            tcp_message->sender()->socket_fd()));
    tcp_message->sender()->SendMessage("Thanks for message, closing your connection.");
    return tcp_util::ACTION_ON_CONNECTION::CLOSE;
}

void TCPServerApp::Start() {
    server_->StartListening();
}

void TCPServerApp::Stop() {
    server_->StopListening();
}