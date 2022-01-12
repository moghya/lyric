//
// Created by Shubham Sawant on 31/12/21.
//

#include "tcp_utils/tcp_utils.h"
#include "tcp_client/tcp_client.h"
#include "tcp_server/tcp_server.h"

#include "../third_party/spdlog/include/spdlog/spdlog.h"
#include "../third_party/asyncplusplus/include/async++.h"

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
        tcp_message->sender()->SendMessage("Thanks for message, << " +
                                           tcp_message->message()->data_str() + " >>. Bye :)");
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
    std::function<void(int)> connect_client_cb = [port  = server.port](int index) mutable {
        TCPClient client("127.0.0.1", port, "client_0");
        std::string msg_str = "Hello World__" + std::to_string(index);
        SPDLOG_INFO(fmt::format("Sending message: {}", msg_str));
        if (client.SendMessage(msg_str)) {
            auto message = client.ReceiveMessage(512);
            SPDLOG_INFO(fmt::format("Received message: {}", message->data()));
        } else {
            throw std::string("Could not send message.");
        }
    };
    int number_of_calls = 1000;
    auto start_clients_cb = [number_of_calls, connect_client_cb]() {
        async::parallel_for(async::irange(0, number_of_calls), connect_client_cb);
    };

    async::spawn(server.start_cb);
    async::spawn(start_clients_cb).then(server.stop_cb);
}

int main() {
    spdlog::set_pattern("[%l] [%n] [%A-%d-%m-%Y] [%H:%M:%S] [%z] [%t] %s:%# %v");
    TestTCPClientAndTCPServer();
    return 0;
}