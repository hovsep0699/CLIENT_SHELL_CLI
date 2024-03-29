#ifndef APPLICATION_H
#define APPLICATION_H


#include <algorithm>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <ctime>
#include <iostream>
#include <unordered_map>
#include <poll.h>
#include <cstdlib>
#include <unistd.h>
#ifdef OS_LINUX
#include <sys/wait.h>
#include <strings.h>
#endif
#include <exception>

#ifdef MAX_CONNECTIONS
#undef MAX_CONNECTIONS
#endif
#define MAX_CONNECTIONS 5

#ifdef BUFFER_SIZE
#undef BUFFER_SIZE
#endif
#define BUFFER_SIZE 1024

namespace network
{
    class   client;
    class   server;
    class   connection_socket;
    class   client_cli;

    /*
     * function for print messages for server and client
    */
    void log(const std::string& /* message */ );
}

namespace command
{
    class   command;
    class   server_command_handler;
    class   client_command_handler;
    class   command_handler;
    struct  invalid_arguments;
}


#endif
