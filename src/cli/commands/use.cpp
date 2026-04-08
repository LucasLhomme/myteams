/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** use
*/

#include "../../../include/cli/commands/use.hpp"
#include <iostream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void UseCommand::execute(const std::string& input, Cli::Socket& socket)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_TEAM:") == 0) {
        std::string uuid = response.substr(18);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_team(uuid.c_str());
    } else if (response.find("ERROR_UNKNOWN_CHANNEL:") == 0) {
        std::string uuid = response.substr(21);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_channel(uuid.c_str());
    } else if (response.find("ERROR_UNKNOWN_THREAD:") == 0) {
        std::string uuid = response.substr(20);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_thread(uuid.c_str());
    } else if (response.find("ERROR") != std::string::npos) {
        client_error_unauthorized();
    }
    
    std::cout << response << std::endl;
}