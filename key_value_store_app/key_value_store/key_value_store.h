//
// Created by Shubham Sawant on 12/01/22.
//

#ifndef KEY_VALUE_STORE_KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_KEY_VALUE_STORE_H

#include <mutex>
#include <string>
#include <unordered_map>

#include "proto/key_value_store.pb.h"

class KeyValueStore {
public:
    enum EvictionPolicy {
        kFIFO, kLRU, kMRU,
    };
public:
    KeyValueStore(std::string name,
                  unsigned int store_capacity,
                  EvictionPolicy eviction_policy);
    ~KeyValueStore();

    KeyValueStoreProto::Result Execute(KeyValueStoreProto::Query query);

private:
    KeyValueStoreProto::PutEntryResult  PutEntry(const KeyValueStoreProto::PutEntryArgs& args);
    KeyValueStoreProto::GetEntryResult  GetEntry(const KeyValueStoreProto::GetEntryArgs& args);

private:
    std::string name_;
    unsigned int capacity_;
    EvictionPolicy eviction_policy_;

    unsigned int size_;
    std::unordered_map<std::string,std::string> key_value_map_;
    std::mutex key_value_map_guard_;
};

#endif//KEY_VALUE_STORE_KEY_VALUE_STORE_H
