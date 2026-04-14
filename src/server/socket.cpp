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
    try {
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
    } catch (const std::exception &e) {
        std::cout << "[ERROR] init_socket exception: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Bind server socket to port
 * @return true if successful, false otherwise
 */
bool Server::bind_socket() {
    try {
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(static_cast<uint16_t>(_port));

        return ::bind(_server_socket, reinterpret_cast<sockaddr *>(&server_addr), 
                      sizeof(server_addr)) != -1;
    } catch (const std::exception &e) {
        std::cout << "[ERROR] bind_socket exception: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Start listening for incoming connections
 * @return true if successful, false otherwise
 */
bool Server::listen_socket() {
    try {
        if (::listen(_server_socket, 32) == -1) {
            return false;
        }
        return true;
    } catch (const std::exception &e) {
        std::cout << "[ERROR] listen_socket exception: " << e.what() << std::endl;
        return false;
    }
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

