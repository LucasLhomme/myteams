/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** send
*/

#include "../../../include/cli/commands/send.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void SendCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    (void)context;
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_USER:") == 0) {
        std::string uuid = response.substr(19);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_user(uuid.c_str());
        return;
    }
    
    if (response.find("ERROR") != std::string::npos) {
        client_error_unauthorized();
        return;
    }
    if (response.find("Message sent to") == 0) {
        std::cout << response << std::endl;
    }
}