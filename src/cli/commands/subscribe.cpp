/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** subscribe
*/

#include "../../../include/cli/commands/subscribe.hpp"
#include <iostream>

void SubscribeCommand::execute(const std::string& input, Cli::Socket& socket)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    std::cout << response << std::endl;
}