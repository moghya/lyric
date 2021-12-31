//
// Created by Shubham Sawant on 30/06/21.
//

#ifndef KEY_VALUE_STORE_TCP_SERVER_APP_H
#define KEY_VALUE_STORE_TCP_SERVER_APP_H

#include <string>

#include "../tcp_utils/tcp_message.h"
#include "../tcp_server/tcp_server.h"
#include "../tcp_utils/tcp_utils.h"

/*
 * This is abstract class of an app that can be ran using a TCPServer.
 * */
class TCPServerApp {
public:
    TCPServerApp(std::string , unsigned int port);
    virtual ~TCPServerApp();

    std::string Name() const {
        return name_;
    }

    virtual unsigned int GetMessageBufferCapacity() {
            return 512;
    };

    virtual tcp_util::ACTION_ON_CONNECTION HandleMessage(
            std::shared_ptr<TCPMessage> tcp_message);

    void Start();
protected:
    std::string name_;
    std::shared_ptr<TCPServer> server_;
};

#endif //KEY_VALUE_STORE_TCP_SERVER_APP_H
