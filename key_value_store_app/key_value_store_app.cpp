//
// Created by Shubham Sawant on 30/06/21.
//

#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../tcp_utils/tcp_utils.h"
#include "key_value_store_app.h"

ACTION_ON_CONNECTION  KeyValueStoreApp::HandleMessage(
        std::shared_ptr<TCPMessage> request) {
  auto message = request->message();
  auto client = request->sender();
  PRINT_THREAD_ID std::cout << Name() << ":: Message " << message->data_str() << std::endl;
  Command command = ParseMessage(message->data());
  switch (command.type) {
    case Command::CommandType::PutEntry: {
        PutEntry(command.key, command.value);
        break;
    }
    case Command::CommandType::GetEntry: {
        client->SendMessage(GetEntry(command.key) + "\n");
        break;
    }
    case Command::CommandType::Close: {
        return ACTION_ON_CONNECTION::CLOSE;
    }
    default: {
        client->SendMessage("Invalid result\n");
    }
  }
  return ACTION_ON_CONNECTION::KEEP_OPEN;
}


KeyValueStoreApp::Command KeyValueStoreApp::ParseMessage(char *message_str) {
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
  if (!get_next_token(command)) return Command();
  if (command == "put") {
    std::string key, value;
    if (get_next_token(key) && get_next_token(value)) {
      std::string temp;
      while (get_next_token(temp)) {
        value += " " + temp;
      }
      return Command(Command::CommandType::PutEntry, key, value);
    }
  } else if (command == "get") {
    std::string key;
    if (get_next_token(key)) {
      return Command(Command::CommandType::GetEntry, key);
    }
  } else if (command == "close") {
    return Command(Command::CommandType::Close);
  }
  return Command();
}


std::string KeyValueStoreApp::GetEntry(std::string key) {
  // Could improve locking here only on the key.
  store_lock_.lock();
  std::string value = "KeyNotFound: " + key;
  auto it = store_.find(key);
  if (it!=store_.end()) {
    value = it->second;
    PRINT_THREAD_ID std::cout << "GetEntry [key: " << key << "] "
                              << "=> [value: " << value << "]" << std::endl;
  } else {
    PRINT_THREAD_ID std::cout << "GetEntry [key: " << key << "] "
                              << "=> " << value << std::endl;
  }
  store_lock_.unlock();
  return value;
}

void KeyValueStoreApp::PutEntry(std::string key, std::string value) {
  // Could improve locking here only on the key.
  store_lock_.lock();
  PRINT_THREAD_ID std::cout << "PutEntry [key: " << key
                            << ", value: " << value << "]" << std::endl;
  store_[key] = value;
  store_lock_.unlock();

}