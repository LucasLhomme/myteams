/*
** EPITECH PROJECT, 2026
** my_teams
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

[[maybe_unused]] static std::vector<std::string> split_tab_fields(const std::string &line)
{
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);

    while (std::getline(stream, field, '\t')) {
        fields.push_back(field);
    }
    return fields;
}

void ListCommand::execute(const std::string &input, Cli::Socket &socket,
                         [[maybe_unused]] Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);

    if (args.size() != 1) {
      std::cout << "Usage: /list" << std::endl;
      return;
    }

    socket.send_data(input);

        while (true) {
            std::string line = CommandHandler::receive_response(socket, 500);
            if (line.empty()) break;
            if (line == "\r" || line == "\n" || line == "") continue;

            if (line.find("ERROR") != std::string::npos || line.find("Unauthorized") != std::string::npos) {
                client_error_unauthorized();
                return;
            }

            std::vector<std::string> fields;
            std::string field;
            std::istringstream stream(line);
            while (std::getline(stream, field, '\t')) {
                fields.push_back(field);
            }
            if (fields.empty()) continue;

            if (fields[0] == "TEAM" && fields.size() >= 4) {
                client_print_teams(fields[1].c_str(), fields[2].c_str(), fields[3].c_str());
            } else if (fields[0] == "CHANNEL" && fields.size() >= 4) {
                client_team_print_channels(fields[1].c_str(), fields[2].c_str(), fields[3].c_str());
            } else if (fields[0] == "THREAD" && fields.size() >= 6) {
                client_channel_print_threads(fields[1].c_str(), fields[2].c_str(), 
                                           (time_t)std::stoll(fields[3]), fields[4].c_str(), fields[5].c_str());
            } else if (fields[0] == "REPLY" && fields.size() >= 5) {
                client_thread_print_replies(fields[1].c_str(), fields[2].c_str(),
                                           (time_t)std::stoll(fields[3]), fields[4].c_str());
            }
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }
}