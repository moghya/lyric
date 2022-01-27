//
// Created by Shubham Sawant on 31/12/21.
//

#include "tcp_utils/tcp_utils.h"
#include "tcp_client/tcp_client.h"
#include "tcp_server/tcp_server.h"

#include "../third_party/spdlog/include/spdlog/spdlog.h"

struct TCPServerSetupInfo {
    unsigned int port;
    std::function<void()> start_cb;
    std::function<void()> stop_cb;
};

TCPServerSetupInfo GetTCPServerSetup() {
    srand (time(NULL));
    unsigned int PORT = 9000 + rand()%1000;
    TCPMessage::Handler handler =
            [] (std::shared_ptr<TCPMessage> tcp_message) -> tcp_util::ACTION_ON_CONNECTION {
        SPDLOG_INFO(fmt::format("Received message:{} from:{}",
                     tcp_message->message()->data(),
                     tcp_message->sender()->socket_fd()));
        bool replied = tcp_message->sender()->SendMessage("Thanks for your message: " +
                                           tcp_message->message()->data_str() + ". Bye :)");
        if (!replied) {
            SPDLOG_ERROR(fmt::format("Failed in replying to client: {}", tcp_message->sender()->socket_fd()));
        }
        return tcp_util::ACTION_ON_CONNECTION::CLOSE;
    };

    auto server = std::make_shared<TCPServer>(PORT, handler);
    auto start_listening_cb = [server]() {
        server->StartListening();
    };
    auto stop_server_cb = [server]() {
        server->StopListening();
        SPDLOG_INFO("TestTCPClientAndTCPServer passed.");
    };
    return TCPServerSetupInfo{PORT, start_listening_cb, stop_server_cb};
}

void TestTCPClientAndTCPServer() {
    auto server = GetTCPServerSetup();
    SPDLOG_INFO(fmt::format("PORT: {}", server.port));
    std::atomic<int> test_pass_count(0);
    std::atomic<int> test_fail_count(0);

    std::function<void(int)> connect_client_cb =
        [port  = server.port, &test_pass_count, &test_fail_count] (int index) mutable {
        TCPClient client("127.0.0.1", port, "client_0");
        std::string msg_str = "Hello World__" + std::to_string(index);
        SPDLOG_INFO(fmt::format("Sending message: {}", msg_str));
        if (client.SendMessage(msg_str)) {
            auto message = client.ReceiveMessage(512);
            if (message) {
                test_pass_count++;
                SPDLOG_INFO(fmt::format("Received message: {}", message->data()));
            } else {
                SPDLOG_ERROR("Could not receive message.");
                test_fail_count++;
            }
        } else {
            SPDLOG_ERROR("Could not send message.");
            test_fail_count++;
        }
    };
    int number_of_calls = 500;
    auto run_server_thread = std::thread(server.start_cb);
    run_server_thread.detach();

    std::vector<std::thread> client_threads;
    for(int i=0; i < number_of_calls; i++) {
        client_threads.push_back(std::move(std::thread(connect_client_cb, i)));
    }
    for (auto&& t : client_threads) {
        t.join();
    }
    server.stop_cb();
    SPDLOG_INFO(fmt::format("Total: {}, Passed: {}. Failed: {}", number_of_calls, test_pass_count, test_fail_count));
}

int main() {
    spdlog::set_pattern("[%l] [%n] [%A-%d-%m-%Y] [%H:%M:%S] [%z] [%t] %s:%# %v");
    TestTCPClientAndTCPServer();
    return 0;
}