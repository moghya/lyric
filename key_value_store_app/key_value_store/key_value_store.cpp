//
// Created by Shubham Sawant on 12/01/22.
//

#include <mutex>
#include <string>
#include <unordered_map>

#include "spdlog/spdlog.h"

#include "key_value_store.h"


KeyValueStore::KeyValueStore(
        std::string name, unsigned int capacity,
        EvictionPolicy eviction_policy) :
    name_(name), capacity_(capacity), eviction_policy_(eviction_policy) {
}

KeyValueStore::~KeyValueStore() {

}

std::string KeyValueStore::GetEntry(std::string key) {
    // Could improve locking here only on the key.
    key_value_map_guard_.lock();
    std::string value = "KeyNotFound: " + key;
    auto it = key_value_map_.find(key);
    if (it!=key_value_map_.end()) {
        value = it->second;
    }
    SPDLOG_INFO(fmt::format("GetEntry [key: {}] => [value: {}]", key, value));
    key_value_map_guard_.unlock();
    return value;
}

void KeyValueStore::PutEntry(std::string key, std::string value) {
    // Could improve locking here only on the key.
    key_value_map_guard_.lock();
    SPDLOG_INFO(fmt::format("GetEntry [key: {}] => [value: {}]", key, value));
    key_value_map_[key] = value;
    key_value_map_guard_.unlock();
}