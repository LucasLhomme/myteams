/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** socket header for the graphical client
*/

#ifndef BONUS_SOCKET_HPP_
#define BONUS_SOCKET_HPP_

#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <vector>

class BonusSocket {
public:
    BonusSocket();
    ~BonusSocket();

    bool connect_to(const std::string& ip, int port);
    void close_connection();
    
    bool send_data(const std::string& data);
    std::string receive_line(); // Returns one line if available, else ""
    
    bool is_connected() const { return _sockfd >= 0 && _is_connected; }
    bool has_data();

private:
    int _sockfd;
    bool _is_connected;
    std::string _input_buffer;
};

#endif
