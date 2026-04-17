/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** socket implementation for the graphical client
*/

#include "../include/socket.hpp"
#include <cstring>
#include <iostream>
#include <fcntl.h>

BonusSocket::BonusSocket() : _sockfd(-1), _is_connected(false) {}

BonusSocket::~BonusSocket() {
    close_connection();
}

bool BonusSocket::connect_to(const std::string& ip, int port) {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) return false;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) return false;

    if (connect(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(_sockfd);
        _sockfd = -1;
        return false;
    }

    _is_connected = true;
    
    int flags = fcntl(_sockfd, F_GETFL, 0);
    fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK);
    
    return true;
}

void BonusSocket::close_connection() {
    if (_sockfd >= 0) {
        close(_sockfd);
        _sockfd = -1;
    }
    _is_connected = false;
}

bool BonusSocket::send_data(const std::string& data) {
    if (!is_connected()) return false;
    ssize_t sent = send(_sockfd, data.c_str(), data.length(), MSG_NOSIGNAL);
    return sent >= 0;
}

bool BonusSocket::has_data() {
    if (!is_connected()) return false;
    
    char buffer[4096];
    ssize_t received = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
    
    if (received > 0) {
        buffer[received] = '\0';
        _input_buffer += buffer;
        return true;
    } else if (received == 0) {
        _is_connected = false;
    }
    return false;
}

std::string BonusSocket::receive_line() {
    size_t pos = _input_buffer.find('\n');
    if (pos != std::string::npos) {
        std::string line = _input_buffer.substr(0, pos);
        _input_buffer.erase(0, pos + 1);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        return line;
    }
    return "";
}
