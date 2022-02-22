//
// Created by Shubham Sawant on 30/06/21.
//

#ifndef KEY_VALUE_STORE_KEY_VALUE_STORE_APP_H
#define KEY_VALUE_STORE_KEY_VALUE_STORE_APP_H


#include <memory>
#include <string>
#include <unordered_map>

#include "../../tcp_app_lib/tcp_server_app/tcp_server_app.h"
#include "../key_value_store/key_value_store.h"
#include "../utils/utils.h"
#include "proto/key_value_store_app.pb.h"


/*
 * This is an implementation of TCPServerApp.
 * It's an in memory Key-Value store that can handle TCP Messages and invoke
 * internal methods to store and retrieve values.
 * It can be extended to have eviction implementation.
 * */

class KeyValueStoreApp : public TCPServerApp {
public:
    KeyValueStoreApp(std::string name,
                     unsigned int store_capacity,
                     KeyValueStore::EvictionPolicy eviction_policy,
                     unsigned int port);

    ~KeyValueStoreApp() override;

    unsigned int GetMessageBufferCapacity() override {
        return utils::GetMessageBufferCapacity();
    }

    KeyValueStoreAppProto::Request ParseMessage(char* message_str);

    tcp_util::ACTION_ON_CONNECTION HandleMessage(
            std::shared_ptr<TCPMessage> tcp_message) override;

private:
    std::shared_ptr<KeyValueStore> store_;
};

//-----------------

#endif // KEY_VALUE_STORE_KEY_VALUE_STORE_APP_H
