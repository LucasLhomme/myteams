/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** socket
*/

#include "./cli/socket.hpp"
#include "exception.hpp"

namespace Cli {
    Socket::Socket() : _sockfd(-1)
    {
        _server_addr = {};
    }

    Socket::~Socket()
    {
        close_socket();
    }

    void Socket::create_socket()
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0) {
            throw Exception("Failed to create socket");
        }
    }

    void Socket::create_sockaddr(const std::string& ip, const std::string& port)
    {
        _server_addr.sin_family = AF_INET;
        _server_addr.sin_port = htons(std::stoi(port));

        if (inet_pton(AF_INET, ip.c_str(), &_server_addr.sin_addr) <= 0) {
            throw Exception("Invalid address / Address not supported");
        }
    }

    void Socket::connect_socket()
    {
        if (connect(_sockfd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
            throw Exception("Connection failed");
        }
    }

    void Socket::close_socket()
    {
        if (_sockfd >= 0) {
            close(_sockfd);
            _sockfd = -1;
        }
    }

    void Socket::send_data(const std::string& data)
    {
        if (_sockfd < 0) return;
        ssize_t sent = send(_sockfd, data.c_str(), data.length(), 0);
        if (sent < 0) {
            throw Exception("Failed to send data");
        }
    }

    std::string Socket::receive_data(size_t max_size)
    {
        if (_sockfd < 0) return "";
        std::string buffer(max_size, '\0');
        ssize_t received = recv(_sockfd, &buffer[0], max_size, 0);
        if (received < 0) {
            throw Exception("Failed to receive data");
        }
        buffer.resize(received);
        return buffer;
    }

    int Socket::get_sockfd() const
    {
        return _sockfd;
    }
}
