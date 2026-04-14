/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** help
*/

#include "../../../include/cli/commands/help.hpp"
#include <iostream>

void HelpCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    try {
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    std::cout << response << std::endl;

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}