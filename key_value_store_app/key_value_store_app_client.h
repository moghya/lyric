//
// Created by Shubham Sawant on 22/12/21.
//

#ifndef KEY_VALUE_STORE_KEYVALUESTOREAPPCLIENT_H
#define KEY_VALUE_STORE_KEYVALUESTOREAPPCLIENT_H


#include "../tcp_app_lib/tcp_server_app_client/tcp_server_app_client.h"

class KeyValueStoreAppClient : public TCPServerAppClient {
public:
    KeyValueStoreAppClient(std::string name,
                           std::string ipAddress,
                           unsigned int port,
                           unsigned int numberOfConnections);
    ~KeyValueStoreAppClient();
    std::unique_ptr<Message> Execute(std::string input_command);
    std::string GetEntry(std::string key);
    std::string PutEntry(std::string key, std::string value);
};



#endif//KEY_VALUE_STORE_KEYVALUESTOREAPPCLIENT_H
