//
// Created by Shubham Sawant on 22/12/21.
//

#include "../../third_party/spdlog/include/spdlog/spdlog.h"
#include "../utils/utils.h"
#include "proto/key_value_store_app.pb.h"
#include "key_value_store_app_client.h"

using KeyValueStoreAppProto::Command;
using KeyValueStoreAppProto::CommandType;

KeyValueStoreAppClient::KeyValueStoreAppClient(
        std::string name, std::string ipAddress,
        unsigned int port, unsigned int numberOfConnections) :
    TCPServerAppClient(ipAddress,port,name) {
}

KeyValueStoreAppClient::~KeyValueStoreAppClient() {

}

std::unique_ptr<Message> KeyValueStoreAppClient::Execute(std::string input_command) {
    tcp_client_->SendMessage(input_command);
    return std::move(tcp_client_->ReceiveMessage(utils::GetMessageBufferCapacity()).result_);
}

std::string KeyValueStoreAppClient::GetEntry(std::string key) {
    Command command;
    command.set_type(CommandType::GetEntry);
    command.set_key(key);
    auto send_res = tcp_client_->SendMessage(command.SerializeAsString());
    if (send_res.success_) {
        auto recv_res = tcp_client_->ReceiveMessage(utils::GetMessageBufferCapacity());
        if (recv_res.success_) {
            return recv_res.result_->data_str();
        } else {
            SPDLOG_ERROR(fmt::format("Receive failed due to error: {}", recv_res.error_.to_string()));
        }
    } else {
        SPDLOG_ERROR(fmt::format("Send failed due to error: {}", send_res.error_.to_string()));
    }
    return "";
}
std::string KeyValueStoreAppClient::PutEntry(std::string key, std::string value) {
    Command command;
    command.set_type(CommandType::PutEntry);
    command.set_key(key);
    command.set_value(value);
    auto send_res = tcp_client_->SendMessage(command.SerializeAsString());
    if (send_res.success_) {
        auto recv_res = tcp_client_->ReceiveMessage(utils::GetMessageBufferCapacity());
        if (recv_res.success_) {
            return recv_res.result_->data_str();
        } else {
            SPDLOG_ERROR(fmt::format("Receive failed due to error: {}", recv_res.error_.to_string()));
        }
    } else {
        SPDLOG_ERROR(fmt::format("Send failed due to error: {}", send_res.error_.to_string()));
    }
    return "";
}