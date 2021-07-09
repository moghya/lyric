//
// Created by Shubham Sawant on 30/06/21.
//

#ifndef KEY_VALUE_STORE_TCP_SERVER_APP_H
#define KEY_VALUE_STORE_TCP_SERVER_APP_H

#include "../tcp_utils/tcp_utils.h"
#include <string>

/*
 * This is abstract class of an app that can be ran using a TCPServer.
 * */
class TCPServerApp {
protected:
    TCPServerApp(std::string name) : name_(name) {}
    std::string Name() const {
        return name_;
    }
    virtual bool IsOn() = 0;
    virtual size_t GetMessageBufferCapacity() {
      return 512;
    }
    virtual ACTION_ON_CONNECTION HandleMessage(
      std::shared_ptr<Message> message) = 0;
  std::string name_;
};

#endif //KEY_VALUE_STORE_TCP_SERVER_APP_H
