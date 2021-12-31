//
// Created by Shubham Sawant on 30/06/21.
//

#ifndef KEY_VALUE_STORE_KEY_VALUE_STORE_APP_H
#define KEY_VALUE_STORE_KEY_VALUE_STORE_APP_H


#include <memory>
#include <string>
#include <unordered_map>

#include "../tcp_app_lib/tcp_server_app/tcp_server_app.h"
#include "utils.h"


/*
 * This is an implementation of TCPServerApp.
 * It's an in memory Key-Value store that can handle TCP Messages and invoke
 * internal methods to store and retrieve values.
 * It can be extended to have eviction implementation.
 * */

class KeyValueStoreApp : public TCPServerApp {
public:
    class Command;
    enum EvictionPolicy {
        kFIFO, kLRU, kMRU,
    };

    KeyValueStoreApp(std::string name,
                     unsigned int store_capacity,
                     EvictionPolicy eviction_policy,
                     unsigned int port);

    ~KeyValueStoreApp() override;

    unsigned int GetMessageBufferCapacity() override {
        return kMessageBufferCapacity;
    }

    Command ParseMessage(char* message_str);

    void PutEntry(std::string key, std::string value);
    std::string GetEntry(std::string key);

    tcp_util::ACTION_ON_CONNECTION HandleMessage(
            std::shared_ptr<TCPMessage> tcp_message) override;

private:
    unsigned int store_capacity_;
    unsigned int store_size_;
    EvictionPolicy eviction_policy_;
    std::unordered_map<std::string,std::string> store_;
    std::mutex store_lock_;
};

//-----------------

class KeyValueStoreApp::Command  {
public:
    enum CommandType {
        Invalid,
        PutEntry,
        GetEntry,
        Close
    };
    Command(CommandType type = CommandType::Invalid,
            std::string key = "",
            std::string value = "") : type(type), key(key), value(value) {}
    CommandType type;
    std::string key;
    std::string value;
};

//-----------------


#endif // KEY_VALUE_STORE_KEY_VALUE_STORE_APP_H
