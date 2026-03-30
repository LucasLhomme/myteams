/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** main
*/
#include <iostream>
#include <regex>
#include "./cli/client.hpp"

void print_help()
{
    std::cout << "USAGE: ./myteams_cli ip port" << std::endl;
    std::cout << "ip is the server ip address on which the server socket listens" << std::endl;
    std::cout << "port is the port number on which the server socket listens" << std::endl;
}

bool is_ip(std::string ip)
{
    // Simple regex to validate IP address format
    std::regex ip_regex("^(\\d{1,3}\\.){3}\\d{1,3}$");
    if (!std::regex_match(ip, ip_regex)) {
        std::cerr << "Invalid IP address format" << std::endl;
        return false;
    }
    return true;
}

bool is_port(std::string port_str)
{
    int port = std::stoi(port_str);
    if (port < 1 || port > 65535) {
        std::cerr << "Invalid port number" << std::endl;
        return false;
    }
    return true;
}

int main(int ac, char **av)
{
    if (ac == 2 && (std::string(av[1]) == "--help" || std::string(av[1]) == "-h")) {
        print_help();
        return 0;
    }
    if (ac != 3) {
        print_help();
        return 84;
    }
    if (!is_ip(av[1]) || !is_port(av[2])) {
        return 84;
    }

    Cli::Client client;
    client.launch_client(av[1], av[2]);
    return 0;
}