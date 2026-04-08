/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** send
*/

#include "../../../include/cli/commands/send.hpp"
#include <iostream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void SendCommand::execute(const std::string& input, Cli::Socket& socket)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_USER:") == 0) {
        std::string uuid = response.substr(19);
        client_error_unknown_user(uuid.c_str());
        return;
    }
    
    std::cout << response << std::endl;
}