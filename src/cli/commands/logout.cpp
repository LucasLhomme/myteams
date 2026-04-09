/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** logout
*/

#include "../../../include/cli/commands/logout.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void LogoutCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("LOGOUT_DATA ") == 0) {
        std::istringstream iss(response.substr(12));
        std::string uuid, username;
        iss >> uuid >> username;
        
        if (!uuid.empty() && !username.empty()) {
            context.reset();
            client_event_logged_out(uuid.c_str(), username.c_str());
            return;
        }
    } else if (response.find("Error") != std::string::npos) {
        client_error_unauthorized();
    }
}