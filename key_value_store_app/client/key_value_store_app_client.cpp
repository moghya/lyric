//
// Created by Shubham Sawant on 22/12/21.
//

#include "../../third_party/spdlog/include/spdlog/spdlog.h"
#include "../utils/utils.h"
#include "proto/key_value_store_app.pb.h"
#include "key_value_store_app_client.h"

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
    return std::move(tcp_client_->ReceiveMessage(utils::GetMessageBufferCapacity()));
}

std::string KeyValueStoreAppClient::GetEntry(std::string key) {
    KeyValueStoreAppCommand command;
    command.set_type(KeyValueStoreAppCommandType::GetEntry);
    command.set_key(key);
    if (tcp_client_->SendMessage(command.SerializeAsString())) {
        auto message = tcp_client_->ReceiveMessage(utils::GetMessageBufferCapacity());
        if (message) {
            return message->data_str();
        } else {
            SPDLOG_ERROR("No message from app.");
        }
    } else {
        SPDLOG_ERROR("Failed to send message");
    }
    return "";
}
std::string KeyValueStoreAppClient::PutEntry(std::string key, std::string value) {
    KeyValueStoreAppCommand command;
    command.set_type(KeyValueStoreAppCommandType::PutEntry);
    command.set_key(key);
    command.set_value(value);
    if (tcp_client_->SendMessage(command.SerializeAsString())) {
        auto message = tcp_client_->ReceiveMessage(utils::GetMessageBufferCapacity());
        if (message) {
            return message->data_str();
        } else {
            SPDLOG_ERROR("No message from app.");
        }
    } else {
        SPDLOG_ERROR("Failed to send message");
    }
    return "";
}