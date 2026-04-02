/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** subscribed
*/

#include "../../../include/cli/commands/subscribed.hpp"
#include <iostream>

void SubscribedCommand::execute(const std::string& input, Cli::Socket& socket)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    std::cout << response << std::endl;
}