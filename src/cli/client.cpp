/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** client
*/
#include "./cli/client.hpp"
#include "./cli/commands.hpp"
#include "./cli/socket.hpp"
#include "exception.hpp"
#include <iostream>

namespace Cli {
    void Client::launch_client(std::string ip, std::string port)
    {
        CommandHandler command_handler;
        Socket client_socket;

        try {
            client_socket.create_socket();
            std::cout << "Client socket created successfully" << std::endl;
            
            client_socket.create_sockaddr(ip, port);
            client_socket.connect_socket();
            std::cout << "Client socket connected successfully to " << ip << ":" << port << std::endl;
        } catch (const Exception& e) {
            std::cerr << e.what() << std::endl;
            exit (84);
        }

        while (true) {
            std::string input;
            if (!std::getline(std::cin, input)) {
                break;
            }
            command_handler.process_command(input, client_socket);
        }
        client_socket.close_socket();
        std::cout << "Client socket closed successfully" << std::endl;
    }
}