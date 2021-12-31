//
// Created by Shubham Sawant on 30/12/21.
//

#include <iostream>
#include "tcp_server_app.h"

const unsigned int PORT = 8081;

int main() {
    auto app = std::make_shared<TCPServerApp>("my_app", PORT);
    app->Start();
    return 0;
}