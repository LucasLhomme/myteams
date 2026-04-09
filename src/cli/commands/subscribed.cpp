/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** subscribed
*/

#include "../../../include/cli/commands/subscribed.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void SubscribedCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_TEAM:") == 0) {
        std::string uuid = response.substr(18);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_team(uuid.c_str());
    } else if (response.find("ERROR") != std::string::npos) {
        client_error_unauthorized();
    } else {
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
    }
}