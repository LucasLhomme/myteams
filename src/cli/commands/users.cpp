/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** users
*/

#include "../../../include/cli/commands/users.hpp"
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

void UsersCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    (void)context;
    try {
        socket.send_data(input);
        
        // Read all available lines (server sends all users in one go, usually followed by quiescence)
        // We wait a bit (100ms) for the server to finish sending.
        bool found_any = false;
        while (true) {
            std::string line = socket.receive_data(100);
            if (line.empty()) break;
            
            if (line.find("ERROR") != std::string::npos || line.find("Unauthorized") != std::string::npos) {
                client_error_unauthorized();
                return;
            }

            std::vector<std::string> fields = split_tab_fields(line);
            if (fields.size() >= 4 && fields[0] == "USER") {
                client_print_users(fields[1].c_str(), fields[2].c_str(), std::stoi(fields[3]));
                found_any = true;
            }
        }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }
}