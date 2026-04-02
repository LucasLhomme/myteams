/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** server
*/

#include "server/server.hpp"
extern "C" {
#include "../../libs/myteams/logging_server.h"
}

#include <iostream>
#include <cstring>
#include <cerrno>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

Server::Server(int port) : _port(port)
{
    init_commands();
}
Server::~Server()
{
    cleanup();
}

bool Server::init_socket()
{
    _server_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket == -1)
        return false;

    int enable = 1;
    if (::setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        ::close(_server_socket);
        _server_socket = -1;
        return false;
    }
    return true;
}

bool Server::bind_socket()
{
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(static_cast<uint16_t>(_port));

    return ::bind(_server_socket, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) != -1;
}

bool Server::listen_socket()
{
    if (::listen(_server_socket, 32) == -1)
        return false;
    return true;
}

void Server::accept_client()
{
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    
    int client_socket = ::accept(_server_socket, reinterpret_cast<sockaddr *>(&client_addr), &client_len);
    if (client_socket != -1) {
        _pollfds.push_back({client_socket, POLLIN, 0});
        _client_buffers[client_socket] = "";
        std::cout << "New client connected on socket " << client_socket << std::endl;
    }
}

void Server::send_response(int client_socket, const std::string &message)
{
    std::string msg_to_send = message;
    if (msg_to_send.size() < 2 || msg_to_send.substr(msg_to_send.size() - 2) != "\r\n") {
        msg_to_send += "\r\n";
    }
    ::send(client_socket, msg_to_send.c_str(), msg_to_send.length(), MSG_NOSIGNAL);
}

void Server::remove_client(int client_socket)
{
    auto it = _socket_to_name.find(client_socket);
    if (it != _socket_to_name.end()) {
        server_event_user_logged_out(it->second.c_str());
        _socket_to_name.erase(it);
    }
    ::close(client_socket);
    _client_buffers.erase(client_socket);
    
    _pollfds.erase(
        std::remove_if(_pollfds.begin(), _pollfds.end(),
            [client_socket](const struct pollfd &pfd) { return pfd.fd == client_socket; }),
        _pollfds.end()
    );
    std::cout << "Client " << client_socket << " disconnected" << std::endl;
}



void Server::extract_and_process_commands(int client_socket)
{
    auto &buffer = _client_buffers[client_socket];
    size_t pos;

    while ((pos = buffer.find("\r\n")) != std::string::npos) {
        std::string command = buffer.substr(0, pos);
        buffer.erase(0, pos + 2);

        while (!command.empty() && std::isspace(static_cast<unsigned char>(command.back())))
            command.pop_back();

        if (!command.empty()) {
            process_command(client_socket, command);
        }
    }
    if (buffer.size() > 4096) {
        buffer.clear();
    }
}

void Server::handle_client_data(size_t poll_index)
{
    int client_socket = _pollfds[poll_index].fd;
    char chunk[1024];
    ssize_t bytes_read = ::recv(client_socket, chunk, sizeof(chunk), 0);

    if (bytes_read <= 0) {
        remove_client(client_socket);
    } else {
        _client_buffers[client_socket].append(chunk, bytes_read);
        extract_and_process_commands(client_socket);
    }
}

void Server::cleanup()
{
    for (const auto &[client_socket, buffer] : _client_buffers) {
        ::close(client_socket);
    }
    _client_buffers.clear();

    if (_server_socket != -1) {
        ::close(_server_socket);
        _server_socket = -1;
    }
    _pollfds.clear();
}

void Server::start()
{
    try {
        if (!init_socket() || !bind_socket() || !listen_socket()) {
            std::cerr << "[ERROR] Initialization failed: " << strerror(errno) << std::endl;
            cleanup();
            return;
        }

        std::cout << "Server listening on port " << _port << std::endl;

        _pollfds.push_back({_server_socket, POLLIN, 0});

        while (true) {
            int poll_ret = ::poll(_pollfds.data(), _pollfds.size(), -1);
            if (poll_ret < 0) {
                std::cerr << "[ERROR] Poll failed: " << strerror(errno) << std::endl;
                break;
            }

            for (int i = static_cast<int>(_pollfds.size()) - 1; i >= 0; i--) {
                if (_pollfds[i].revents & POLLIN) {
                    if (_pollfds[i].fd == _server_socket) {
                        accept_client();
                    } else {
                        handle_client_data(i);
                    }
                }
            }
        }
        cleanup();
    } catch (const std::exception &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        cleanup();
    }
}
