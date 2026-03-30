/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** server implementation
*/

#include <csignal>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <poll.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "server.hpp"

Server::Server(int port) : _port(port), _server_socket(-1)
{
}

Server::~Server()
{
    cleanup();
}

void Server::send(int client_socket, const std::string &message)
{
    ::send(client_socket, message.c_str(), message.length(), MSG_NOSIGNAL);
}

bool Server::init_socket()
{
    _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket == -1)
        return false;

    int enable = 1;
    if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &enable,
            sizeof(enable)) == -1) {
        close(_server_socket);
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

    if (bind(_server_socket, reinterpret_cast<sockaddr *>(&server_addr),
            sizeof(server_addr)) == -1)
        return false;
    return true;
}

bool Server::listen_socket()
{
    if (listen(_server_socket, 32) == -1)
        return false;
    return true;
}

int Server::accept_client()
{
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    
    int client_socket = accept(_server_socket, 
        reinterpret_cast<sockaddr *>(&client_addr),
        &client_len);
    return client_socket;
}

std::string Server::read_command(int client_socket)
{
    char buffer[256];
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::string command(buffer);
        size_t end = command.find_last_not_of(" \n\r\t");
        if (end != std::string::npos)
            command = command.substr(0, end + 1);
        else
            return "";
        return command;
    }
    return "";
}

void Server::handle_command(int client_socket, const std::string &command)
{
    if (command == "exit") {
        send(client_socket, "Goodbye!\n");
        handle_client_disconnect(client_socket);
    } else if (command == "help") {
        send(client_socket, "Commands: exit, help\n");
    } else {
        send(client_socket, "Unknown command\n");
    }
}

int Server::setup_poll_fds(struct pollfd *fds)
{
    int index = 1;
    
    memset(fds, 0, sizeof(struct pollfd) * 500);
    fds[0].fd = _server_socket;
    fds[0].events = POLLIN;
    
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] != -1) {
            fds[index].fd = _clients[i];
            fds[index].events = POLLIN;
            index++;
        }
    }
    return index;
}

void Server::handle_client_disconnect(int client_fd)
{
    close(client_fd);
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] == client_fd) {
            _clients[i] = -1;
            break;
        }
    }
}

void Server::handle_clients_input(struct pollfd *fds)
{
    int pollfd_idx = 1;
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] != -1) {
            if (fds[pollfd_idx].revents & POLLIN) {
                std::string cmd = read_command(_clients[i]);
                if (cmd.empty()) {
                    handle_client_disconnect(_clients[i]);
                } else {
                    handle_command(_clients[i], cmd);
                }
            }
            pollfd_idx++;
        }
    }
}

void Server::cleanup()
{
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] != -1)
            close(_clients[i]);
    }
    if (_server_socket != -1)
        close(_server_socket);
}

void Server::start()
{
    if (!init_socket() || !bind_socket() || !listen_socket()) {
        cleanup();
        return;
    }

    std::cout << "Server listening on port " << _port << std::endl;

    struct pollfd *fds = new struct pollfd[500];
    int index;

    while (true) {
        index = setup_poll_fds(fds);
        if (poll(fds, index, -1) < 0)
            break;

        if (fds[0].revents & POLLIN) {
            int client_socket = accept_client();
            if (client_socket != -1) {
                _clients.push_back(client_socket);
                std::cout << "New client connected" << std::endl;
            }
        }
        handle_clients_input(fds);

        for (int i = _clients.size() - 1; i >= 0; i--) {
            if (_clients[i] == -1)
                _clients.erase(_clients.begin() + i);
        }
    }

    delete[] fds;
    cleanup();
}



