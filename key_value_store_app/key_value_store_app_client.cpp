//
// Created by Shubham Sawant on 22/12/21.
//

#include "proto/key_value_store_app.pb.h"
#include "key_value_store_app_client.h"
#include "utils.h"

using key_value_store_app::KeyValueStoreAppCommand;
using key_value_store_app::KeyValueStoreAppCommandType;

KeyValueStoreAppClient::KeyValueStoreAppClient(
        std::string name, std::string ipAddress,
        unsigned int port, unsigned int numberOfConnections) :
    TCPServerAppClient(ipAddress,port,name) {
}

KeyValueStoreAppClient::~KeyValueStoreAppClient() {

}

std::unique_ptr<Message> KeyValueStoreAppClient::Execute(std::string input_command) {
    tcp_client_->SendMessage(input_command);
    return std::move(tcp_client_->ReceiveMessage(kMessageBufferCapacity));
}

std::string KeyValueStoreAppClient::GetEntry(std::string key) {
    KeyValueStoreAppCommand command;
    command.set_type(KeyValueStoreAppCommandType::GetEntry);
    command.set_key(key);
    tcp_client_->SendMessage(command.SerializeAsString());
    return tcp_client_->ReceiveMessage(kMessageBufferCapacity)->data_str();
}
std::string KeyValueStoreAppClient::PutEntry(std::string key, std::string value) {
    KeyValueStoreAppCommand command;
    command.set_type(KeyValueStoreAppCommandType::PutEntry);
    command.set_key(key);
    command.set_value(value);
    tcp_client_->SendMessage(command.SerializeAsString());
    return tcp_client_->ReceiveMessage(kMessageBufferCapacity)->data_str();
}