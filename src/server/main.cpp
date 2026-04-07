/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** server main
*/

#include <iostream>
#include <cstdlib>
#include <csignal>
#include "./server/server.hpp"

bool server_running = true;

void signal_handler(int sig)
{
    (void)sig;
    server_running = false;
}

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cerr << "Usage: ./my_teams <port>" << std::endl;
        return 84;
    }
    int port = std::atoi(av[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Invalid port" << std::endl;
        return 84;
    }
    std::signal(SIGINT, signal_handler);
    Server server(port);
    server.start();
    return 0;
}
