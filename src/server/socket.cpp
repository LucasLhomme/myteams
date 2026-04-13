/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** socket and network helpers with clean poll management
*/

#include "server/server.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

/**
 * @brief Initialize server socket with SO_REUSEADDR option
 * @return true if successful, false otherwise
 */
bool Server::init_socket() {
    _server_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket == -1) {
        return false;
    }

    int enable = 1;
    if (::setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        ::close(_server_socket);
        _server_socket = -1;
        return false;
    }
    return true;
}

/**
 * @brief Bind server socket to port
 * @return true if successful, false otherwise
 */
bool Server::bind_socket() {
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(static_cast<uint16_t>(_port));

    return ::bind(_server_socket, reinterpret_cast<sockaddr *>(&server_addr), 
                  sizeof(server_addr)) != -1;
}

/**
 * @brief Start listening for incoming connections
 * @return true if successful, false otherwise
 */
bool Server::listen_socket() {
    if (::listen(_server_socket, 32) == -1) {
        return false;
    }
    return true;
}

/**
 * @brief Create a pollfd structure for monitoring a file descriptor
 * @param fd File descriptor to monitor
 * @return pollfd structure initialized with POLLIN event
 */
struct pollfd Server::make_pollfd(int fd) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    return pfd;
}

