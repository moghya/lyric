//
// Created by Shubham Sawant on 22/12/21.
//

#include "../../third_party/spdlog/include/spdlog/spdlog.h"
#include "../utils/utils.h"
#include "key_value_store_app_client.h"
#include "proto/key_value_store_app.pb.h"

using KeyValueStoreAppProto::Request;
using KeyValueStoreAppProto::Response;
using KeyValueStoreAppProto::RequestType;

KeyValueStoreAppClient::KeyValueStoreAppClient(
        std::string name, std::string ipAddress,
        unsigned int port, unsigned int numberOfConnections) :
    TCPServerAppClient(ipAddress,port,name) {
}

KeyValueStoreAppClient::~KeyValueStoreAppClient() {

}

Response KeyValueStoreAppClient::Execute(Request request) {
    Response response;
    auto send_res = tcp_client_->SendMessage(request.SerializeAsString());
    if (!send_res.success_) {
        SPDLOG_ERROR(fmt::format("Send failed due to error: {}", send_res.to_string()));
        response.set_error(KeyValueStoreAppProto::kNetworkTransportError);
        return response;
    }

    auto recv_res = tcp_client_->ReceiveMessage(utils::GetMessageBufferCapacity());
    if (!recv_res.success_) {
        response.set_error(KeyValueStoreAppProto::kNetworkTransportError);
        SPDLOG_ERROR(fmt::format("Receive failed, recv_res: {}", recv_res.to_string()));
    } else if(!response.ParseFromString(recv_res.result_->data_str())) {
        response.set_error(KeyValueStoreAppProto::kInvalidRequest);
        SPDLOG_ERROR(fmt::format("Invalid response: {}", recv_res.result_->data_str()));
    }
    return response;
}

Response KeyValueStoreAppClient::GetEntry(std::string key) {
    Request request;
    request.set_type(RequestType::GetEntry);

    auto* query = request.mutable_query();
    query->set_operation(KeyValueStoreProto::GetEntry);

    auto* args = query->mutable_get_entry_args();
    args->set_key(key);

    auto res = Execute(request);
    return res;
}

Response KeyValueStoreAppClient::PutEntry(std::string key, std::string value) {
    Request request;
    request.set_type(RequestType::PutEntry);

    auto* query = request.mutable_query();
    query->set_operation(KeyValueStoreProto::PutEntry);

    auto* args = query->mutable_put_entry_args();
    args->set_key(key);
    args->set_value(value);

    auto res = Execute(request);
    return res;
}