/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** list
*/

#include "../../../include/cli/commands/list.hpp"
#include <iostream>
#include <sstream>
#include <vector>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

static std::vector<std::string> split_tab_fields(const std::string &line)
{
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);

    while (std::getline(stream, field, '\t')) {
        fields.push_back(field);
    }
    return fields;
}

void ListCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
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
    } else if (response == "No items available") {
        return;
    } else {
        std::istringstream iss(response);
        std::string line;

        while (std::getline(iss, line)) {
            if (line.empty()) {
                continue;
            }

            const std::vector<std::string> fields = split_tab_fields(line);
            if (fields.empty()) {
                continue;
            }

            if (fields[0] == "TEAM" && fields.size() >= 4) {
                client_print_teams(fields[1].c_str(), fields[2].c_str(), fields[3].c_str());
            } else if (fields[0] == "CHANNEL" && fields.size() >= 4) {
                client_team_print_channels(fields[1].c_str(), fields[2].c_str(), fields[3].c_str());
            } else if (fields[0] == "THREAD" && fields.size() >= 6) {
                client_channel_print_threads(
                    fields[1].c_str(), fields[2].c_str(), static_cast<time_t>(std::stoll(fields[3])),
                    fields[4].c_str(), fields[5].c_str());
            } else if (fields[0] == "REPLY" && fields.size() >= 5) {
                client_thread_print_replies(
                    fields[1].c_str(), fields[2].c_str(), static_cast<time_t>(std::stoll(fields[3])),
                    fields[4].c_str());
            }
        }
    }
}