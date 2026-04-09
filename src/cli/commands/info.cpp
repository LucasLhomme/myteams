/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** info
*/

#include "../../../include/cli/commands/info.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void InfoCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
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
    } else {
        if (response.find("USER") != std::string::npos) {
            std::istringstream iss(response);
            std::string type, uuid, name, status_str;
            iss >> type >> uuid >> name >> status_str;
            int status = (status_str == "1") ? 1 : 0;
        } else if (response.find("TEAM") != std::string::npos) {
            //client_print_team(uuid.c_str(), name.c_str(), desc.c_str());
        } else if (response.find("CHANNEL") != std::string::npos) {
            //client_print_channel(uuid.c_str(), name.c_str(), desc.c_str());
        } else if (response.find("THREAD") != std::string::npos) {
            //client_print_thread(uuid.c_str(), user_uuid.c_str(), timestamp, title.c_str(), body.c_str());
        }
    }
}