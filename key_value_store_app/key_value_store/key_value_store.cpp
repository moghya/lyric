//
// Created by Shubham Sawant on 12/01/22.
//

#include <mutex>
#include <string>
#include <unordered_map>

#include "proto/key_value_store.pb.h"
#include "spdlog/spdlog.h"

#include "key_value_store.h"


using KeyValueStoreProto::GetEntryArgs;
using KeyValueStoreProto::GetEntryResult;
using KeyValueStoreProto::PutEntryArgs;
using KeyValueStoreProto::PutEntryResult;
using KeyValueStoreProto::Query;
using KeyValueStoreProto::Result;


KeyValueStore::KeyValueStore(
        std::string name, unsigned int capacity,
        EvictionPolicy eviction_policy) :
    name_(name), capacity_(capacity), eviction_policy_(eviction_policy) {
}

KeyValueStore::~KeyValueStore() {

}

Result KeyValueStore::Execute(Query query) {
    Result result;
    result.set_operation(query.operation());
    switch(query.operation()) {
        case KeyValueStoreProto::GetEntry: {
            auto res = GetEntry(query.get_entry_args());
            result.mutable_get_entry_result()->CopyFrom(res);
            result.set_error(res.error());
            break;
        }
        case KeyValueStoreProto::PutEntry: {
            auto res = PutEntry(query.put_entry_args());
            result.mutable_put_entry_result()->CopyFrom(res);
            result.set_error(res.error());
            break;
        }
    }
    return result;
}

GetEntryResult KeyValueStore::GetEntry(const GetEntryArgs& args) {
    // Could improve locking here only on the key.
    auto& key = args.key();
    auto result = GetEntryResult();
    result.set_error(KeyValueStoreProto::ErrorType::kNoError);
    key_value_map_guard_.lock();
    auto it = key_value_map_.find(key);
    if (it!=key_value_map_.end()) {
        result.set_value(it->second);
    } else {
        result.set_error(KeyValueStoreProto::ErrorType::kKeyNotFound);
    }
    key_value_map_guard_.unlock();
    return result;
}

PutEntryResult KeyValueStore::PutEntry(const PutEntryArgs& args) {
    // Could improve locking here only on the key.
    auto key = args.key();
    auto value = args.value();
    auto result = PutEntryResult();
    result.set_error(KeyValueStoreProto::ErrorType::kNoError);
    result.set_key(key);
    result.set_value(value);
    key_value_map_guard_.lock();
    key_value_map_[key] = value;
    key_value_map_guard_.unlock();
    return result;
}