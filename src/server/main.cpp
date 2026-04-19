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

void signal_handler([[maybe_unused]] int sig)
{
    server_running = false;
}

void print_help()
{
    std::cout << "USAGE: ./myteams_server port" << std::endl;
    std::cout << "\tport is the port number on which the server socket listens." << std::endl;
}

int main(int ac, char **av)
{
    try {
        if (ac == 2 && (std::string(av[1]) == "--help" || std::string(av[1]) == "-h")) {
            print_help();
            return 0;
        }
        if (ac != 2) {
            print_help();
            return 84;
        }
        int port = std::atoi(av[1]);
        if (port <= 0 || port > 65535) {
            std::cout << "Invalid port" << std::endl;
            return 84;
        }
        std::signal(SIGINT, signal_handler);
        Server server(port);
        server.start();
    } catch (const std::exception &e) {
        std::cout << "Fatal error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}
