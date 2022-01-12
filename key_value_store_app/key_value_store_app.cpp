//
// Created by Shubham Sawant on 30/06/21.
//

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../third_party/spdlog/include/spdlog/spdlog.h"
#include "../tcp_app_lib/tcp_utils/tcp_utils.h"
#include "key_value_store_app.h"

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
  SPDLOG_INFO(fmt::format("Received Message: {}",message->data_str()));
  KeyValueStore::Command command = ParseMessage(message->data());
  switch (command.type) {
    case KeyValueStore::Command::CommandType::PutEntry: {
      store_->PutEntry(command.key, command.value);
      client->SendMessage("[" + command.key + ":" + store_->GetEntry(command.key) + "]\n");
      break;
    }
    case KeyValueStore::Command::CommandType::GetEntry: {
      client->SendMessage(store_->GetEntry(command.key) + "\n");
      break;
    }
    case KeyValueStore::Command::CommandType::Close: {
      client->SendMessage("closed_by-> " + Name());
      return tcp_util::ACTION_ON_CONNECTION::CLOSE;
    }
    default: {
        client->SendMessage("Invalid result\n");
    }
  }
  return tcp_util::ACTION_ON_CONNECTION::KEEP_OPEN;
}


KeyValueStore::Command KeyValueStoreApp::ParseMessage(char *message_str) {
  // Extract as a util class Tokenizer with following method
  // get_next_token, reset_iterator, get_first, get_last etc..
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
  if (!get_next_token(command)) return KeyValueStore::Command();
  if (command == "put") {
    std::string key, value;
    if (get_next_token(key) && get_next_token(value)) {
      std::string temp;
      while (get_next_token(temp)) {
        value += " " + temp;
      }
      return KeyValueStore::Command(KeyValueStore::Command::CommandType::PutEntry, key, value);
    }
  } else if (command == "get") {
    std::string key;
    if (get_next_token(key)) {
      return KeyValueStore::Command(KeyValueStore::Command::CommandType::GetEntry, key);
    }
  } else if (command == "close") {
    return KeyValueStore::Command(KeyValueStore::Command::CommandType::Close);
  }
  return KeyValueStore::Command();
}