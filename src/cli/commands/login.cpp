/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** login
*/

#include "../../../include/cli/commands/login.hpp"
#include <iostream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void LoginCommand::execute(const std::string &input, Cli::Socket &socket,
                           Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);

    if (args.size() != 2) {
      std::cout << "Usage: /login [\"user_name\"]" << std::endl;
      return;
    }

    socket.send_data(input);
    std::string response = socket.receive_data();

    if (response.find("Login success:") != std::string::npos) {
      std::string uuid = response.substr(response.find(':') + 1);
      uuid.erase(0, uuid.find_first_not_of(" \n\r\t"));
      uuid.erase(uuid.find_last_not_of("\n\r\t") + 1);

      context.set_user(uuid, args[1]);
      client_event_logged_in(uuid.c_str(), args[1].c_str());
    } else if (response.find("ERROR") != std::string::npos) {
      std::cout << response << std::endl;
    }

  } catch (const std::exception &e) {
    std::cout << "[ERROR] " << e.what() << std::endl;
  }
}