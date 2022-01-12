//
// Created by Shubham Sawant on 12/01/22.
//

#ifndef KEY_VALUE_STORE_KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_KEY_VALUE_STORE_H

#include <mutex>
#include <string>
#include <unordered_map>

class KeyValueStore {
public:
    class Command;
    enum EvictionPolicy {
        kFIFO, kLRU, kMRU,
    };
public:
    KeyValueStore(std::string name,
                  unsigned int store_capacity,
                  EvictionPolicy eviction_policy);
    ~KeyValueStore();

    void PutEntry(std::string key, std::string value);
    std::string GetEntry(std::string key);
private:
    std::string name_;
    unsigned int capacity_;
    EvictionPolicy eviction_policy_;

    unsigned int size_;
    std::unordered_map<std::string,std::string> key_value_map_;
    std::mutex key_value_map_guard_;
};

//-----------------

class KeyValueStore::Command  {
public:
    enum CommandType {
        Invalid,
        PutEntry,
        GetEntry,
        Close
    };
    Command(CommandType type = CommandType::Invalid,
            std::string key = "",
            std::string value = "") : type(type), key(key), value(value) {}
    CommandType type;
    std::string key;
    std::string value;
};


#endif//KEY_VALUE_STORE_KEY_VALUE_STORE_H
