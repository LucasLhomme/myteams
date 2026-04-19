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
    try {
    socket.send_data(input);
    std::string response = socket.receive_data();
    
    if (response.find("USER_LOGGED_OUT") != std::string::npos || response.find("LOGOUT_DATA") != std::string::npos) {
        std::istringstream iss(response);
        std::string tag, uuid, username;
        iss >> tag >> uuid >> username;
        
        if (!uuid.empty()) {
            context.reset();
            client_event_logged_out(uuid.c_str(), username.empty() ? "" : username.c_str());
            return;
        }
    } else if (response.find("ERROR") != std::string::npos || response.find("Error") != std::string::npos || response.find("Unauthorized") != std::string::npos) {
        client_error_unauthorized();
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}