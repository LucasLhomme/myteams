/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** messages
*/

#include "../../../include/cli/commands/messages.hpp"
#include <iostream>

void MessagesCommand::execute(const std::string& input, Cli::Socket& socket)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    std::cout << response << std::endl;
}