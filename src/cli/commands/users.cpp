/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** users
*/

#include "../../../include/cli/commands/users.hpp"
#include <sstream>
#include <iostream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void UsersCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    try {
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    std::istringstream iss(response);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty() || line.find("USER") != 0) continue;
        
        std::istringstream entry(line);
        std::string type, uuid, name, status_str;
        entry >> type >> uuid >> name >> status_str;
        
        if (!uuid.empty() && !name.empty()) {
            int status = (status_str == "1" || status_str == "online") ? 1 : 0;
            client_print_users(uuid.c_str(), name.c_str(), status);
        }
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}