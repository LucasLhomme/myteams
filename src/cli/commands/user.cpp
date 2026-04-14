/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** user
*/

#include "../../../include/cli/commands/user.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void UserCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    try {
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_USER:") == 0) {
        std::string uuid = response.substr(19);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_user(uuid.c_str());
        return;
    }
    // STATUS: 1 = online, 0 = offline
    std::istringstream iss(response);
    std::string uuid, name, status_str;
    iss >> uuid >> name >> status_str;
    
    if (!uuid.empty() && !name.empty()) {
        int status = (status_str == "1" || status_str == "online") ? 1 : 0;
        client_print_user(uuid.c_str(), name.c_str(), status);
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}