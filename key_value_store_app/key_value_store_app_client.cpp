//
// Created by Shubham Sawant on 22/12/21.
//

#include "key_value_store_app_client.h"
#include "utils.h"

KeyValueStoreAppClient::KeyValueStoreAppClient(
        std::string name, std::string ipAddress,
        unsigned int port, unsigned int numberOfConnections) :
    TCPServerAppClient(ipAddress,port,name) {
}

std::unique_ptr<Message> KeyValueStoreAppClient::Execute(std::string input_command) {
    tcp_client_->SendMessage(input_command);
    return std::move(tcp_client_->ReceiveMessage(kMessageBufferCapacity));
}

KeyValueStoreAppClient::~KeyValueStoreAppClient() {

}