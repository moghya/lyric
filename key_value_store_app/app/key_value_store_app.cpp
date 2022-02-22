//
// Created by Shubham Sawant on 30/06/21.
//

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../../tcp_app_lib/tcp_utils/tcp_utils.h"
#include "key_value_store_app.h"
#include "proto/key_value_store_app.pb.h"
#include "spdlog/spdlog.h"

using KeyValueStoreAppProto::Request;
using KeyValueStoreAppProto::Response;
using KeyValueStoreAppProto::RequestType;

KeyValueStoreApp::KeyValueStoreApp(
    std::string name,
    unsigned int store_capacity,
    KeyValueStore::EvictionPolicy eviction_policy,
    unsigned int port) :
   TCPServerApp(name, port),
   store_(std::make_shared<KeyValueStore>(name, store_capacity, eviction_policy)) {
}

KeyValueStoreApp::~KeyValueStoreApp() {
}

tcp_util::ACTION_ON_CONNECTION
KeyValueStoreApp::HandleMessage(std::shared_ptr<TCPMessage> tcp_message) {
  auto message = tcp_message->message();
  auto client = tcp_message->sender();
  SPDLOG_DEBUG(fmt::format("Received Message: {}", message->data_str()));

  Request request;
  Response response;
  response.set_error(KeyValueStoreAppProto::kNoError);
  if(!request.ParseFromString(message->data())) {
      request.set_type(RequestType::Unknown);
    SPDLOG_ERROR(fmt::format("Failed to parse message: {}", message->data_str()));
  } else {
    SPDLOG_DEBUG(fmt::format("Received Request: {}", request.ShortDebugString()));
  }
  response.set_req_type(request.type());

  auto action = tcp_util::ACTION_ON_CONNECTION::KEEP_OPEN;
  switch (request.type()) {
    case RequestType::PutEntry:
    case RequestType::GetEntry: {
        const auto& result = store_->Execute(request.query());
        response.mutable_query_result()->CopyFrom(result);
        break;
    }
    case RequestType::Close: {
        action = tcp_util::ACTION_ON_CONNECTION::CLOSE;
        break;
    }
    default: {
        response.set_error(KeyValueStoreAppProto::kInvalidRequest);
    }
  }
  SPDLOG_DEBUG(fmt::format("Sending response: {}", response.SerializeAsString()));
  client->SendMessage(response.SerializeAsString());
  return action;
}


Request KeyValueStoreApp::ParseMessage(char *message_str) {
  throw "Deprecated method used";
  // Extract as a util class Tokenizer with following method
  // get_next_token, reset_iterator, get_first, get_last etc..
  Request keyValueStoreAppCommand;
  keyValueStoreAppCommand.set_type(RequestType::Unknown);
  std::vector<std::string> tokens;
  std::function<void(char*, const char*, std::vector<std::string>&)>
      tokenize = [] (char* text, const char* delimeter,
                     std::vector<std::string>& tokens) {
    char* token = strtok(text, delimeter);
    while (token) {
      std::string str_token = token;
      str_token.erase(0, str_token.find_first_not_of("\t\n\v\f\r "));
      str_token.erase(str_token.find_last_not_of("\t\n\v\f\r ") + 1);
      tokens.push_back(str_token);
      token = strtok(NULL, delimeter);
    }
  };

  tokenize(message_str, " ", tokens);
  std::vector<std::string>::iterator token_iterator = tokens.begin();
  std::function<bool(std::string& str)> get_next_token =
  [&](std::string& str) mutable {
    if (token_iterator == tokens.end()) return false;
    str = *token_iterator;
    std::advance(token_iterator,1);
    return true;
  };

  std::string command;
  if (!get_next_token(command)) return keyValueStoreAppCommand;
  if (command == "put") {
    std::string key, value;
    if (get_next_token(key) && get_next_token(value)) {
      std::string temp;
      while (get_next_token(temp)) {
        value += " " + temp;
      }
      keyValueStoreAppCommand.set_type(RequestType::PutEntry);
    }
  } else if (command == "get") {
    std::string key;
    if (get_next_token(key)) {
        keyValueStoreAppCommand.set_type(RequestType::PutEntry);
    }
  } else if (command == "close") {
    keyValueStoreAppCommand.set_type(RequestType::Close);
  }
  return keyValueStoreAppCommand;
}