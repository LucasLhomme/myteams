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

static std::string trim_copy(const std::string &value)
{
    const size_t first = value.find_first_not_of(" \n\r\t");

    if (first == std::string::npos) {
        return "";
    }
    const size_t last = value.find_last_not_of(" \n\r\t");

    return value.substr(first, last - first + 1);
}

void SubscribedCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    try {
    socket.send_data(input + "\n");
    while (true) {
        std::string line = CommandHandler::receive_response(socket, 500);
        if (line.empty()) break;
        if (line == "\r" || line == "\n" || line == "") continue;

        if (line.find("ERROR_UNKNOWN_TEAM:") == 0) {
            std::string uuid = trim_copy(line.substr(std::string("ERROR_UNKNOWN_TEAM:").size()));
            client_error_unknown_team(uuid.c_str());
            return;
        } else if (line.find("ERROR") != std::string::npos) {
            client_error_unauthorized();
            return;
        } else if (line.find("NO_TEAMS") == 0 || line.find("NO_USERS") == 0) {
            continue;
        }

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

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}