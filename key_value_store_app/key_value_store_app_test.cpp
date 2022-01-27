//
// Created by Shubham Sawant on 14/01/22.
//


#include <memory>


#include "../third_party/spdlog/include/spdlog/spdlog.h"

#include "app/key_value_store_app.h"
#include "client/key_value_store_app_client.h"

struct KeyValueStoreAppSetupInfo {
    unsigned int port;
    std::shared_ptr<KeyValueStoreApp> app;
    std::function<void()> start_cb;
    std::function<void()> stop_cb;
};

KeyValueStoreAppSetupInfo GetKeyValueStoreAppSetupInfo() {
    srand (time(NULL));
    unsigned int PORT = 9000 + rand()%1000;
    const unsigned int STORE_CAPACITY = 1000;
    auto app = std::make_shared<KeyValueStoreApp>("my-key-value-store-"+std::to_string(PORT),
                                                  STORE_CAPACITY,
                                                  KeyValueStore::EvictionPolicy::kFIFO,
                                                  PORT);
    auto start_app_cb = [app]() {
        SPDLOG_INFO("Starting app.");
        app->Start();
    };
    auto stop_app_cb = [app]() {
        SPDLOG_INFO("Stopping app.");
        app->Stop();
    };
    return KeyValueStoreAppSetupInfo{PORT, app, start_app_cb, stop_app_cb};
}

void TestKeyValueStoreAppAndClient() {
    auto app_setup_info = GetKeyValueStoreAppSetupInfo();
    SPDLOG_INFO(fmt::format("PORT: {}", app_setup_info.port));
    std::atomic<int> test_pass_count(0);
    std::atomic<int> test_fail_count(0);
    std::function<void(int)> client_perform_read_write_cb =
        [port  = app_setup_info.port, &test_pass_count, &test_fail_count] (int index) mutable {
        auto client = std::make_shared<KeyValueStoreAppClient>("my_client", "127.0.0.1", port, 1);
        auto key = std::to_string(index);
        auto value = "hello_for_" + key;

        SPDLOG_INFO(fmt::format("Performing GetKey key: [{}]", key));
        auto response = client->GetEntry(key);
        SPDLOG_INFO(fmt::format("Did GetKey key: [{}] ~ response: [{}]", key, response));

        SPDLOG_INFO(fmt::format("Performing PutKey key: [{}],  value:[{}]", key, value));
        response = client->PutEntry(key, value);
        SPDLOG_INFO(fmt::format("Did PutKey key: [{}], value:[{}] ~ response: [{}]", key, value, response));

        SPDLOG_INFO(fmt::format("Performing GetKey key: [{}]", key));
        response = client->GetEntry(key);
        SPDLOG_INFO(fmt::format("Did GetKey key: [{}] ~ response: [{}]", key, response));

        if (value!=response) {
            test_fail_count++;
            SPDLOG_ERROR(fmt::format("Response is not as expected [value!=response] ~ [{}!={}]", value, response));
        } else {
            test_pass_count++;
        }
    };

    int number_of_clients = 500;
    auto run_app_thread = std::thread(app_setup_info.start_cb);
    run_app_thread.detach();
    std::vector<std::thread> threads;
    for(int i=0; i<number_of_clients; i++) {
        threads.push_back(std::move(std::thread(client_perform_read_write_cb, i)));
    }
    for (auto&& t : threads) {
        t.join();
    }
    app_setup_info.stop_cb();
    SPDLOG_INFO(fmt::format("Total: {}, Passed: {}. Failed: {}", number_of_clients, test_pass_count, test_fail_count));
}

int main() {
    spdlog::set_pattern("[%l] [%n] [%A-%d-%m-%Y] [%H:%M:%S] [%z] [%t] %s:%# %v");
    TestKeyValueStoreAppAndClient();
    return 0;
}