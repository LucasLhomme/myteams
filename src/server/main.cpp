/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** server main
*/

#include <iostream>
#include <cstdlib>
#include "./server/server.hpp"

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

    Server server(port);
    server.start();

    return 0;
}
