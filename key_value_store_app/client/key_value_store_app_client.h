//
// Created by Shubham Sawant on 22/12/21.
//

#ifndef KEY_VALUE_STORE_KEYVALUESTOREAPPCLIENT_H
#define KEY_VALUE_STORE_KEYVALUESTOREAPPCLIENT_H


#include "../../tcp_app_lib/tcp_server_app_client/tcp_server_app_client.h"
#include "proto/key_value_store_app.pb.h"


class KeyValueStoreAppClient : public TCPServerAppClient {
public:
    KeyValueStoreAppClient(std::string name,
                           std::string ipAddress,
                           unsigned int port,
                           unsigned int numberOfConnections);
    ~KeyValueStoreAppClient();
    KeyValueStoreAppProto::Response GetEntry(std::string key);
    KeyValueStoreAppProto::Response PutEntry(std::string key, std::string value);
private:
    KeyValueStoreAppProto::Response Execute(KeyValueStoreAppProto::Request request);
};



#endif//KEY_VALUE_STORE_KEYVALUESTOREAPPCLIENT_H
