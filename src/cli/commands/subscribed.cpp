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
    try {
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_TEAM:") == 0) {
        std::string uuid = response.substr(18);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_team(uuid.c_str());
    } else if (response.find("ERROR") != std::string::npos) {
        client_error_unauthorized();
    } else if (response.find("NO_TEAMS") == 0 || response.find("NO_USERS") == 0) {
        // Empty list - no output needed
    } else {
        while (!response.empty()) {
            std::istringstream iss(response);
            std::string line;
            while (std::getline(iss, line)) {
                if (line.empty()) continue;

                size_t first_pipe = line.find('|');
                size_t second_pipe = line.find('|', first_pipe + 1);
                size_t third_pipe = line.find('|', second_pipe + 1);

                if (first_pipe == std::string::npos || second_pipe == std::string::npos) continue;

                std::string type = line.substr(0, first_pipe);
                std::string uuid = line.substr(first_pipe + 1, second_pipe - first_pipe - 1);
                std::string name = line.substr(second_pipe + 1, third_pipe - second_pipe - 1);

                if (type == "TEAM" && third_pipe != std::string::npos) {
                    std::string description = line.substr(third_pipe + 1);
                    description.erase(description.find_last_not_of(" \n\r\t") + 1);
                    client_print_teams(uuid.c_str(), name.c_str(), description.c_str());
                } else if (type == "USER" && third_pipe != std::string::npos) {
                    std::string status_str = line.substr(third_pipe + 1);
                    status_str.erase(status_str.find_last_not_of(" \n\r\t") + 1);
                    int status = std::stoi(status_str);
                    client_print_users(uuid.c_str(), name.c_str(), status);
                }
            }

            response = socket.receive_data(0);
        }
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}