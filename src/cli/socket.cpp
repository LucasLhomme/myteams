/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** socket implementation with poll for non-blocking I/O
*/

#include "./cli/socket.hpp"
#include "exception.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#include <cerrno>
#include <cstring>

namespace Cli {
    Socket::Socket() : _sockfd(-1), _is_connected(false), _server_addr({})
    {
    }

    Socket::~Socket()
    {
        close_socket();
    }

    void Socket::create_socket()
    {
        try {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0) {
                throw Exception("Failed to create socket");
            }
        } catch (const std::exception &e) {
            std::cout << "[ERROR] create_socket: " << e.what() << std::endl;
            throw;
        }
    }

    void Socket::create_sockaddr(const std::string& ip, const std::string& port)
    {
        try {
            _server_addr.sin_family = AF_INET;
            _server_addr.sin_port = htons(std::stoi(port));

            if (inet_pton(AF_INET, ip.c_str(), &_server_addr.sin_addr) <= 0) {
                throw Exception("Invalid address / Address not supported");
            }
        } catch (const std::exception &e) {
            std::cout << "[ERROR] create_sockaddr: " << e.what() << std::endl;
            throw;
        }
    }

    void Socket::connect_socket()
    {
        try {
            if (connect(_sockfd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
                throw Exception("Connection failed");
            }
            _is_connected = true;
        } catch (const std::exception &e) {
            std::cout << "[ERROR] connect_socket: " << e.what() << std::endl;
            throw;
        }
    }

    void Socket::close_socket()
    {
        try {
            if (_sockfd >= 0) {
                close(_sockfd);
                _sockfd = -1;
            }
            _is_connected = false;
        } catch (const std::exception &e) {
            std::cout << "[ERROR] close_socket: " << e.what() << std::endl;
        }
    }

    int Socket::_poll_socket(short events, int timeout_ms)
    {
        if (_sockfd < 0) {
            return -1;
        }

        struct pollfd pfd;
        pfd.fd = _sockfd;
        pfd.events = events;
        pfd.revents = 0;

        int poll_ret = poll(&pfd, 1, timeout_ms);
        if (poll_ret < 0 && errno != EINTR) {
            return -1;
        }
        if (poll_ret == 0) {
            return 0;
        }
        return pfd.revents;
    }

    bool Socket::_internal_send()
    {
        if (_output_buffer.empty() || _sockfd < 0) {
            return true;
        }

        ssize_t sent = send(_sockfd, _output_buffer.c_str(), _output_buffer.length(), MSG_NOSIGNAL);
        if (sent < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                throw Exception("Failed to send data");
            }
            return false;
        }

        _output_buffer.erase(0, sent);
        return _output_buffer.empty();
    }

    bool Socket::_internal_recv()
    {
        if (_sockfd < 0) {
            return false;
        }

        char chunk[4096];
        ssize_t received = recv(_sockfd, chunk, sizeof(chunk), 0);

        if (received < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                throw Exception("Failed to receive data");
            }
            return false;
        }

        if (received == 0) {
            _is_connected = false;
            throw Exception("Connection closed by server");
        }

        _input_buffer.append(chunk, received);
        return true;
    }

    bool Socket::send_data(const std::string& data)
    {
        try {
            if (_sockfd < 0) {
                return false;
            }

            _output_buffer += data;

            int revents = _poll_socket(POLLOUT, 1000);
            if (revents < 0) {
                throw Exception("Poll failed for writing");
            }

            if (revents & POLLOUT) {
                return _internal_send();
            }

            return false;
        } catch (const std::exception &e) {
            std::cout << "[ERROR] send_data: " << e.what() << std::endl;
            return false;
        }
    }

    std::string Socket::receive_data(int timeout_ms)
    {
        try {
            if (_sockfd < 0) {
                return "";
            }

            const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);

            while (true) {
                size_t newline_pos = _input_buffer.find('\n');
                if (newline_pos != std::string::npos) {
                    std::string result = _input_buffer.substr(0, newline_pos);
                    _input_buffer.erase(0, newline_pos + 1);
                    if (!result.empty() && result.back() == '\r') {
                        result.pop_back();
                    }
                    return result;
                }

                const auto now = std::chrono::steady_clock::now();
                if (now >= deadline) {
                    return "";
                }

                const int remaining_ms = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now).count();
                int revents = _poll_socket(POLLIN, remaining_ms);
                if (revents < 0) {
                    throw Exception("Poll failed for reading");
                }
                if (revents & POLLIN) {
                    _internal_recv();
                }
            }
        } catch (const std::exception &e) {
            std::cout << "[ERROR] receive_data: " << e.what() << std::endl;
            return "";
        }
    }

    bool Socket::is_readable(int timeout_ms)
    {
        if (_sockfd < 0 || !_is_connected) {
            return false;
        }

        int revents = _poll_socket(POLLIN, timeout_ms);
        if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
            _is_connected = false;
            return false;
        }
        return (revents > 0) && (revents & POLLIN);
    }

    bool Socket::is_writable(int timeout_ms)
    {
        if (_sockfd < 0) {
            return false;
        }

        int revents = _poll_socket(POLLOUT, timeout_ms);
        return (revents > 0) && (revents & POLLOUT);
    }

    int Socket::get_sockfd() const
    {
        return _sockfd;
    }

    bool Socket::is_connected() const
    {
        return _sockfd >= 0;
    }
}
