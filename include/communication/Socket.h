#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <string>

// Different types of sockets. Each is represented by its own child class.
enum class SocketType
{
    IP,
    BLUETOOTH
};

// A unix socket
class Socket
{
public:
    // Constructor
    Socket() = default;

    int socket_fd;
    void* address;
    size_t address_len;
    char* buffer; // Message buffer
};

#endif
