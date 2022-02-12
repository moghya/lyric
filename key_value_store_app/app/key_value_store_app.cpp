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

using KeyValueStoreAppProto::Command;
using KeyValueStoreAppProto::CommandType;

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
KeyValueStoreApp::HandleMessage(std::shared_ptr<TCPMessage> request) {
  auto message = request->message();
  auto client = request->sender();
  SPDLOG_INFO(fmt::format("Received Message: {}", message->data_str()));
  Command command;
  if(!command.ParseFromString(message->data())) {
      command.set_type(CommandType::Unknown);
      SPDLOG_ERROR(fmt::format("Failed to parse message: {}", message->data_str()));
  }
  SPDLOG_INFO(fmt::format("Received Command: [{}]", command.ShortDebugString()));
  switch (command.type()) {
    case CommandType::PutEntry: {
      store_->PutEntry(command.key(), command.value());
      auto response = fmt::format("[{}:{}]", command.key(), store_->GetEntry(command.key()));
      SPDLOG_INFO("Sending: " + response);
      client->SendMessage(response);
      break;
    }
    case CommandType::GetEntry: {
      auto value = store_->GetEntry(command.key());
      SPDLOG_INFO("Sending: " + value);
      client->SendMessage(value);
      break;
    }
    case CommandType::Close: {
        client->SendMessage("closed_by-> " + Name());
        return tcp_util::ACTION_ON_CONNECTION::CLOSE;
    }
    default: {
        client->SendMessage("Invalid request.");
    }
  }
  return tcp_util::ACTION_ON_CONNECTION::KEEP_OPEN;
}


Command KeyValueStoreApp::ParseMessage(char *message_str) {
  // Extract as a util class Tokenizer with following method
  // get_next_token, reset_iterator, get_first, get_last etc..
  Command keyValueStoreAppCommand;
  keyValueStoreAppCommand.set_type(CommandType::Unknown);
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
      keyValueStoreAppCommand.set_type(CommandType::PutEntry);
      keyValueStoreAppCommand.set_key(key);
      keyValueStoreAppCommand.set_value(value);
    }
  } else if (command == "get") {
    std::string key;
    if (get_next_token(key)) {
        keyValueStoreAppCommand.set_type(CommandType::PutEntry);
        keyValueStoreAppCommand.set_key(key);
    }
  } else if (command == "close") {
    keyValueStoreAppCommand.set_type(CommandType::Close);
  }
  return keyValueStoreAppCommand;
}