//
// Created by Shubham Sawant on 08/07/21.
//

#ifndef KEY_VALUE_STORE_TCP_UTILS_H
#define KEY_VALUE_STORE_TCP_UTILS_H

#include <iostream>
#include <memory>
#include <thread>

#include "message.h"
#include "tcp_connection.h"
#include "tcp_message.h"

#define PRINT_THREAD_ID do { \
   std::cout << "Thread::" \
             << std::this_thread::get_id() << "::" << __FUNCTION__ \
             << "\t"; \
  } while(false); \

enum ACTION_ON_CONNECTION {
    KEEP_OPEN,
    CLOSE,
};

#endif // KEY_VALUE_STORE_TCP_UTILS_H
