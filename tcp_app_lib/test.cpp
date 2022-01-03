//
// Created by Shubham Sawant on 31/12/21.
//

#include "tcp_utils/tcp_utils.h"
#include "tcp_client/tcp_client.h"
#include "tcp_server/tcp_server.h"
#include "tcp_server_app/tcp_server_app.h"
#include "tcp_server_app_client/tcp_server_app_client.h"


void TestTCPClientAndTCPServer() {
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
    tcp_util::SpawnThread(start_listening_cb, false /* join */);
    std::atomic<unsigned int> count(0);
    auto connect_client_cb = [server, PORT, &count]() mutable {
        count++;
        TCPClient client("127.0.0.1", PORT, "client_0");
        std::string msg_str = "Hello World__" + std::to_string(count);
        PRINT_THREAD_ID std::cout << "Sending message: " << msg_str << "\n";
        if (client.SendMessage(msg_str)) {
            auto message = client.ReceiveMessage(512);
            PRINT_THREAD_ID std::cout << "Got message from server: " <<  message->data() << "\n";
        } else {
            throw std::string("Could not send message.");
        }
    };
    const unsigned int number_of_calls = 1000;
    for(unsigned int i=0; i < number_of_calls; ++i) {
        tcp_util::SpawnThread(connect_client_cb);
    }
    auto stop_server_cb = [server]() {
        server->StopListening();
        PRINT_THREAD_ID std::cout << "TestTCPClientAndTCPServer passed.\n";
    };
    tcp_util::SpawnThread(stop_server_cb);
}

int main() {
    TestTCPClientAndTCPServer();
    return 0;
}