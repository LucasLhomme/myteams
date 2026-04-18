/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** use
*/

#include "../../../include/cli/commands/use.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

static std::string trim_copy(const std::string &value) {
  const size_t first = value.find_first_not_of(" \n\r\t");

  if (first == std::string::npos) {
    return "";
  }
  const size_t last = value.find_last_not_of(" \n\r\t");

  return value.substr(first, last - first + 1);
}

void UseCommand::execute(const std::string &input, Cli::Socket &socket,
                         Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);
    if (args.size() > 4) {
      std::cout << "Usage: /use ?[\"team_uuid\"] ?[\"channel_uuid\"] ?[\"thread_uuid\"]" << std::endl;
      return;
    }

    socket.send_data(input);
    std::string response = socket.receive_data();

    if (response.find("ERROR_UNKNOWN_TEAM:") == 0) {
      std::string uuid =
          trim_copy(response.substr(std::string("ERROR_UNKNOWN_TEAM:").size()));
      client_error_unknown_team(uuid.c_str());
    } else if (response.find("ERROR_UNKNOWN_CHANNEL:") == 0) {
      std::string uuid = trim_copy(
          response.substr(std::string("ERROR_UNKNOWN_CHANNEL:").size()));
      client_error_unknown_channel(uuid.c_str());
    } else if (response.find("ERROR_UNKNOWN_THREAD:") == 0) {
      std::string uuid = trim_copy(
          response.substr(std::string("ERROR_UNKNOWN_THREAD:").size()));
      client_error_unknown_thread(uuid.c_str());
    } else if (response.find("ERROR") != std::string::npos) {
      if (response.find("Unauthorized") != std::string::npos) {
        client_error_unauthorized();
      } else {
        std::cout << response << std::endl;
      }
    } else {
      const std::vector<std::string> args = split_args(input);
      if (args.size() > 1) context.set_team(args[1]);
      if (args.size() > 2) context.set_channel(args[2]);
      if (args.size() > 3) context.set_thread(args[3]);
      std::cout << response << std::endl;
    }

  } catch (const std::exception &e) {
    std::cout << "[ERROR] " << e.what() << std::endl;
  }
}