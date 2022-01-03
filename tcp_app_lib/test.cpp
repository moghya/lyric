//
// Created by Shubham Sawant on 31/12/21.
//

#include "tcp_utils/tcp_utils.h"
#include "tcp_client/tcp_client.h"
#include "tcp_server/tcp_server.h"
#include "tcp_server_app/tcp_server_app.h"
#include "tcp_server_app_client/tcp_server_app_client.h"

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
        PRINT_THREAD_ID
        std::cout << "Received message: " << tcp_message->message()->data()
                  << " from: " << tcp_message->sender()->socket_fd() << "\n";
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
        PRINT_THREAD_ID std::cout << "TestTCPClientAndTCPServer passed.\n";
    };
    return TCPServerSetupInfo{PORT, start_listening_cb, stop_server_cb};
}

void TestTCPClientAndTCPServer() {
    auto server = GetTCPServerSetup();
    std::function<void(int)> connect_client_cb = [port  = server.port](int index) mutable {
        TCPClient client("127.0.0.1", port, "client_0");
        std::string msg_str = "Hello World__" + std::to_string(index);
        PRINT_THREAD_ID std::cout << "Sending message: " << msg_str << "\n";
        if (client.SendMessage(msg_str)) {
            auto message = client.ReceiveMessage(512);
            PRINT_THREAD_ID std::cout << "Got message from server: " <<  message->data() << "\n";
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
    TestTCPClientAndTCPServer();
    return 0;
}