/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** user
*/

#include "../../../include/cli/commands/user.hpp"
#include <iostream>
#include <sstream>
#include <vector>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void UserCommand::execute(const std::string &input, Cli::Socket &socket,
                          [[maybe_unused]] Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);

    if (args.size() != 2) {
      std::cout << "Usage: /user [\"user_uuid\"]" << std::endl;
      return;
    }

    socket.send_data(input);
    std::string response = socket.receive_data();

    if (response.find("ERROR_UNKNOWN_USER:") == 0) {
      std::string uuid = response.substr(19);
      uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
      client_error_unknown_user(uuid.c_str());
      return;
    }

    if (response.find("ERROR") != std::string::npos) {
      if (response.find("Unauthorized") != std::string::npos) {
        client_error_unauthorized();
      } else {
        std::cout << response << std::endl;
      }
      return;
    }
    // STATUS: 1 = online, 0 = offline
    std::istringstream entry(response);
    std::string first;
    entry >> first;
    
    std::string uuid, name, status_str;
    if (first == "USER") {
        entry >> uuid >> name >> status_str;
    } else {
        uuid = first;
        entry >> name >> status_str;
    }
    
    if (!uuid.empty() && !name.empty()) {
        int status = (status_str == "1" || status_str == "online") ? 1 : 0;
        client_print_user(uuid.c_str(), name.c_str(), status);
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}