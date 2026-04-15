/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** login
*/

#include "../../../include/cli/commands/login.hpp"
#include <iostream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void LoginCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    try {
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("Login success:") == 0) {
        std::string uuid = response.substr(14);
        uuid.erase(0, uuid.find_first_not_of(" \n\r\t"));
        uuid.erase(uuid.find_last_not_of("\n\r\t") + 1);
        const std::vector<std::string> args = split_args(input);

        if (args.size() >= 2) {
            context.set_user(uuid, args[1]);
            client_event_logged_in(uuid.c_str(), args[1].c_str());
        }
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}