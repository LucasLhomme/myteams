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


void UsersCommand::execute(const std::string &input, Cli::Socket &socket,
                           Cli::Context &context) {
  (void)context;
  try {
    const std::vector<std::string> args = split_args(input);

    if (args.size() != 1) {
      std::cout << "Usage: /users" << std::endl;
      return;
    }

    socket.send_data(input);
        
        // Read all available lines (server sends all users in one go, usually followed by quiescence)
        // We wait a bit (100ms) for the server to finish sending.
        bool found_any = false;
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

            if (fields.size() >= 4 && fields[0] == "USER") {
                client_print_users(fields[1].c_str(), fields[2].c_str(), std::stoi(fields[3]));
                found_any = true;
            }
        }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }
}